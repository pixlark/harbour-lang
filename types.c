#include "types.h"

const char * type_str[] = {
	"i32"
};
const char * operator_str[] = {
	"NEG",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
};
const char * expr_str[] = {
	"ATOM",
	"VAR",
	"UNARY",
	"BINARY",
	"FUNCALL",
};

char * str_expr(Expr * expr)
{
	char buffer[512];
	switch (expr->type) {
	case EXPR_ATOM: {
		sprintf(buffer, "Atom: %d", expr->atom.val);
	} break;
	case EXPR_VAR: {
		sprintf(buffer, "Var: %s", expr->var.name);
	} break;
	case EXPR_UNARY: {
		char * operand = str_expr(expr->unary.operand);
		sprintf(buffer, "Unary %s on [%s]", operator_str[expr->unary.operator], operand);
		free(operand);
	} break;
	case EXPR_BINARY: {
		char * left = str_expr(expr->binary.left);
		char * right = str_expr(expr->binary.right);
		sprintf(buffer, "Binary %s on [%s], [%s]",
			operator_str[expr->binary.operator],
			left, right);
		free(right);
		free(left);
	} break;
 	case EXPR_FUNCALL: {
		sprintf(buffer, "Funcall %s on [",
			expr->funcall.name);
		for (int i = 0; i < sb_count(expr->funcall.args); i++) {
			char * arg = str_expr(expr->funcall.args[i]);
			strcat(buffer, arg);
			if (i != sb_count(expr->funcall.args) - 1) {
				strcat(buffer, ", ");
			}
			free(arg);
		}
		strcat(buffer, "]");
	} break;
	}
	char * str = malloc(strlen(buffer) + 1);
	strcpy(str, buffer);
	return str;
}

void print_expr(Expr * expr)
{
	char * str = str_expr(expr);
	printf("%s\n", str);
	free(str);
}

char * str_stmt(Stmt * stmt)
{
	char buffer[512];
	switch (stmt->type) {
	case STMT_EXPR: {
		char * expr = str_expr(stmt->expr.expr);
		strcpy(buffer, expr);
		free(expr);
	} break;
	case STMT_LET: {
		sprintf(buffer, "let %s : %s", stmt->let.name, type_str[stmt->let.type]);
		if (stmt->let.expr) {
			char * expr = str_expr(stmt->let.expr);
			strcat(buffer, " = ");
			strcat(buffer, expr);
			free(expr);
		}
		strcat(buffer, ";");
	} break;
	case STMT_PRINT: {
		char * expr = str_expr(stmt->print.expr);
		sprintf(buffer, "print %s;", expr);
		free(expr);
	} break;
	}
	char * str = malloc(strlen(buffer) + 1);
	strcpy(str, buffer);
	return str;
}

void print_stmt(Stmt * stmt)
{
	char * str = str_stmt(stmt);
	printf("%s\n", str);
	free(str);
}

