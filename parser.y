%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "stretchy_buffer.h"
	int yylex();
	void yyerror(char const *);
	void init();
%}

%code requires {
	typedef struct Str_Intern {
		size_t len;
		const char * str;
	} Str_Intern;
	Str_Intern * str_interns;
	const char * str_intern_range(const char * start, const char * end);
	const char * str_intern(const char * str);

	typedef enum {
		OP_NEG,
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
	} Operator;
	typedef enum {
		EXPR_ATOM,
		EXPR_VAR,
		EXPR_UNARY,
		EXPR_BINARY,
		EXPR_FUNCALL,
		EXPR_COMMA,
	} Expr_Type ;
	typedef struct Expr {
		Expr_Type type;
		union {
			struct {
				int val;
			} atom;
			struct {
				const char * name;
			} var;
			struct {
				Operator operator;
				struct Expr * operand;
			} unary;
			struct {
				Operator operator;
				struct Expr * left;
				struct Expr * right;
			} binary;
			struct {
				const char * name;
				struct Expr ** args;
			} funcall;
		};
	} Expr;
	void print_expr(Expr * expr);
	#define EXPR(t) \
		Expr * expr = malloc(sizeof(Expr)); \
		expr->type = t;
 }

%define parse.error verbose

%union {
	int val;
	const char * ident;
	Expr * expr;
	Expr ** exprlist;
};
%token <val> INT
%token <ident> IDENT
%type <expr> expression
%type <exprlist> comma_expression

%left '+' '-'
%left '*' '/'
%left UMINUS

%start outer

%%

comma_expression:
expression {
	/*
	EXPR(EXPR_COMMA);
	expr->comma.self = $1;
	expr->comma.next = NULL;
	$$ = expr;*/
	Expr ** list = NULL;
	sb_push(list, $1);
	$$ = list;
}
| comma_expression ',' expression {
	/*
	EXPR(EXPR_COMMA);
	expr->comma.self = $3;
	expr->comma.next = NULL;
	$1->comma.next = expr;
	$$ = $1;*/
	Expr ** list = $1;
	sb_push(list, $3);
	$$ = list;
}
;

expression:
INT {
	EXPR(EXPR_ATOM);
	expr->atom.val = $1;
	$$ = expr;
}
| IDENT {
	EXPR(EXPR_VAR);
	expr->var.name = $1;
	$$ = expr;
}
| IDENT '(' ')' {
	EXPR(EXPR_FUNCALL);
	expr->funcall.name = $1;
	expr->funcall.args = NULL;
	$$ = expr;
}
| IDENT '(' comma_expression ')' {
	EXPR(EXPR_FUNCALL);
	expr->funcall.name = $1;
	expr->funcall.args = $3;
	$$ = expr;
}
| expression '+' expression {
	EXPR(EXPR_BINARY);
	expr->binary.operator = OP_ADD;
	expr->binary.left = $1;
	expr->binary.right = $3;
	$$ = expr;
}
| expression '-' expression {
	EXPR(EXPR_BINARY);
	expr->binary.operator = OP_SUB;
	expr->binary.left = $1;
	expr->binary.right = $3;
	$$ = expr;
}
| expression '*' expression {
	EXPR(EXPR_BINARY);
	expr->binary.operator = OP_MUL;
	expr->binary.left = $1;
	expr->binary.right = $3;
	$$ = expr;
}
| expression '/' expression {
	EXPR(EXPR_BINARY);
	expr->binary.operator = OP_DIV;
	expr->binary.left = $1;
	expr->binary.right = $3;
	$$ = expr;
}
| '-' expression %prec UMINUS {
	EXPR(EXPR_UNARY);
	expr->unary.operator = OP_NEG;
	expr->unary.operand = $2;
	$$ = expr;
}
| '(' expression ')' {
	$$ = $2;
}
;

outer:
expression {
	print_expr($1); 
}
;

%%

const char * str_intern_range(const char * start, const char * end)
{
	size_t len = end - start;
	for (int i = 0; i < sb_count(str_interns); i++) {
		if (str_interns[i].len == len &&
			strncmp(str_interns[i].str, start, len) == 0) {
			return str_interns[i].str;
		}
	}
	char * interned = malloc(len + 1);
	strncpy(interned, start, len);
	interned[len] = '\0';
	Str_Intern new_intern = {len, interned};
	sb_push(str_interns, new_intern);
	return new_intern.str;
}

const char * str_intern(const char * str)
{
	return str_intern_range(str, str + strlen(str));
}

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
		sprintf(buffer, "Unary %d on [%s]", expr->unary.operator, operand);
		free(operand);
	} break;
	case EXPR_BINARY: {
		char * left = str_expr(expr->binary.left);
		char * right = str_expr(expr->binary.right);
		sprintf(buffer, "Binary %d on [%s], [%s]",
			expr->binary.operator,
			left, right);
		free(right);
		free(left);
	} break;
 	case EXPR_FUNCALL: {
		//char * args = str_expr(expr->funcall.args);
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
		//free(args);
	} break;
	/*
	case EXPR_COMMA: {
		strcpy(buffer, "Comma: ");
		Expr * iter = expr;
		while (iter != NULL) {
			char * str = str_expr(iter->comma.self);
			strcat(buffer, "[");
			strcat(buffer, str);
			free(str);
			if (iter->comma.next != NULL) {
				strcat(buffer, "], ");
			} else {
				strcat(buffer, "]");
			}
			iter = iter->comma.next;
		}
	} break;*/
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

int main(int argc, char ** argv)
{
	yydebug = 0;
	init();
	yyparse();
}
