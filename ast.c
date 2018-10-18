#include "ast.h"

void print_symbols(Map * map)
{
	for (int i = 0; i < map->size; i++) {
		if (map->taken[i]) {
			printf("'%s' -> %u\n",
				(char*) map->keys[i],
				(size_t) map->values[i]);
		}
	}
}

void tag_expr(Function * func, Expr * expr)
{
	switch (expr->type) {
	case EXPR_ATOM:
		break;
	case EXPR_VAR: {
		if (!map_index(
			func->symbols,
			(uint64_t) expr->var.name, NULL)) {
			map_insert(
				func->symbols, (uint64_t) expr->var.name,
				func->stack_offset);
			func->stack_offset += sizeof(int32_t);
		}
	} break;
	case EXPR_UNARY:
		tag_expr(func, expr->unary.operand);
		break;
	case EXPR_BINARY:
		tag_expr(func, expr->binary.left);
		tag_expr(func, expr->binary.right);
		break;
	}
}

void tag_stmt(Function * func, Stmt * stmt)
{
	switch (stmt->type) {
	case STMT_EXPR:
		tag_expr(func, stmt->expr.expr);
		break;
	case STMT_PRINT:
		tag_expr(func, stmt->print.expr);
		break;
	}
}

void tag_function_vars(Function * func)
{
	func->symbols = make_map(512);
	func->stack_offset = 0;

	for (int i = 0; i < sb_count(func->stmts); i++) {
		tag_stmt(func, func->stmts[i]);
	}

	printf("Frame size: %u\n", func->stack_offset);
	print_symbols(func->symbols);
}
