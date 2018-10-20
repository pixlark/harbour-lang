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

bool typecheck_expr(Expr * expr, Type expected)
{
	switch (expr->type) {
	case EXPR_ATOM:
		return expr->atom.val_type == expected;
		break;
	case EXPR_VAR:
		
		break;
	}
}
