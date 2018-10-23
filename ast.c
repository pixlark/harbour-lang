#include "ast.h"

void print_symbols(Symbol_Table * table)
{
	printf("-- SYMBOLS --\n");
	for (int i = 0; i < table->size; i++) {
		if (table->taken[i]) {
			printf("'%s' (@%p) -> %s, offset: %d\n",
				table->keys[i],
				table->keys[i],
				type_str[table->values[i].type],
				table->values[i].offset);
		}
	}
	printf("-------------\n");
}

void tag_stmt(Function * func, Stmt * stmt)
{
	if (stmt->type == STMT_LET) {
		if (!table_symbol_exists(func->symbols, stmt->let.name)) {
			table_add_symbol(
				func->symbols,
				stmt->let.name,
				create_symbol(stmt->let.type, func->stack_offset));
			func->stack_offset += sizeof(int32_t);
		}
	}
}

void create_symbols(Function * func)
{
	func->symbols = table_create(512);
	func->stack_offset = 0;

	for (int i = 0; i < sb_count(func->stmts); i++) {
		tag_stmt(func, func->stmts[i]);
	}

	// Retag as offset from FP rather than SP
	for (int i = 0; i < func->symbols->size; i++) {
		if (func->symbols->taken[i]) {
			func->symbols->values[i].offset -= func->stack_offset;
		}
	}
	
	printf("Frame size: %d\n", func->stack_offset);
	print_symbols(func->symbols);
}

Type typecheck_expr(Function * func, Expr * expr)
{
	switch (expr->type) {
	case EXPR_ATOM: {
		return expr->atom.val_type;
	} break;
	case EXPR_VAR: {
		Symbol sym = get_symbol(func, expr->var.name);
		return sym.type;
	} break;
	case EXPR_BINARY: {
		Type left  = typecheck_expr(func, expr->binary.left);
		Type right = typecheck_expr(func, expr->binary.right);
		// Check that operands are the same type
		if (left != right) {
			fatal("Operands of %s not of same type",
				operator_str[expr->binary.operator]);
		}
		// Check that operands are valid for the operator
		if (!(operator_types[expr->binary.operator] & left)) {
			fatal("Operands of type %s not valid for operator %s",
				type_str[left],
				operator_str[expr->binary.operator]);
		}
		return left;
	} break;
	case EXPR_UNARY: {
		Type type = typecheck_expr(func, expr->unary.operand);
		// Check that operand is valid for operator
		if (!(operator_types[expr->unary.operator] & type)) {
			fatal("Operand of type %s not valid for operator %s",
				type_str[type],
				operator_str[expr->unary.operator]);
		}
		return type;
	} break;
	default: {
		fatal("Case not handled!");
	} break;
	}
}

Type typecheck_stmt(Function * func, Stmt * stmt)
{
	switch (stmt->type) {
	case STMT_LET: {
		Type expr_type = typecheck_expr(func, stmt->let.expr);
		if (expr_type != stmt->let.type) {
			fatal("Let expected type %s, got %s",
				type_str[stmt->let.type],
				type_str[expr_type]);
		}
	} break;
	case STMT_ASSIGN: {
		Type expr_type = typecheck_expr(func, stmt->assign.expr);
		Symbol sym = get_symbol(func, stmt->assign.name);
		if (expr_type != sym.type) {
			fatal("Assignment expected type %s, got %s",
				type_str[sym.type],
				type_str[expr_type]);
		}
	} break;
	default:
		break;
	}
}
