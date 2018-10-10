%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "stretchy_buffer.h"
	int yylex();
	//void yyerror(char const *);
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
		TYPE_I32,
	} Type;
	const char * type_str[];
	typedef enum {
		OP_NEG,
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
	} Operator;
	const char * operator_str[];
	typedef enum {
		EXPR_ATOM,
		EXPR_VAR,
		EXPR_UNARY,
		EXPR_BINARY,
		EXPR_FUNCALL,
	} Expr_Type;
	const char * expr_str[];
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
	typedef enum {
		STMT_EXPR,
		STMT_LET,
		STMT_PRINT,
	} Stmt_Type;
	typedef struct Stmt {
		Stmt_Type type;
		union {
			struct {
				Expr * expr;
			} expr;
			struct {
				const char * name;
				Type type;
				Expr * expr;
			} let;
			struct {
				Expr * expr;
			} print;
		};
	} Stmt;
	void print_expr(Expr * expr);
	void print_stmt(Stmt * stmt);
	#define EXPR(t) \
		Expr * expr = malloc(sizeof(Expr)); \
		expr->type = t;
	#define STMT(t) \
		Stmt * stmt = malloc(sizeof(Stmt)); \
		stmt->type = t;
	Stmt ** parse();
	void yyerror(Stmt *** program_stmts, char const * s);
 }

%define parse.error verbose
%parse-param {Stmt *** program_stmts}
						
%union {
	int val;
	const char * ident;
	Expr * expr;
	Expr ** exprlist;
	Stmt * stmt;
	Type type;
};

%token         LET_KW
%token         PRINT_KW
%token <val>   INT_LIT
%token <ident> IDENT
%token <type>  TYPE

%type <expr>     expression
%type <exprlist> comma_expression
%type <stmt>     statement
%type <stmt>     let_stmt
%type <stmt>     print_stmt

%left '+' '-'
%left '*' '/'
%left UMINUS

%start program

%%

comma_expression:
expression {
	Expr ** list = NULL;
	sb_push(list, $1);
	$$ = list;
}
| comma_expression ',' expression {
	Expr ** list = $1;
	sb_push(list, $3);
	$$ = list;
}
;

expression:
INT_LIT {
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

let_stmt:
LET_KW IDENT ':' TYPE {
	STMT(STMT_LET);
	stmt->let.name = $2;
	stmt->let.type = $4;
	stmt->let.expr = NULL;
	$$ = stmt;
}
| LET_KW IDENT ':' TYPE '=' expression {
	STMT(STMT_LET);
	stmt->let.name = $2;
	stmt->let.type = $4;
	stmt->let.expr = $6;
	$$ = stmt;
}
;

print_stmt:
PRINT_KW expression {
	STMT(STMT_PRINT);
	stmt->print.expr = $2;
	$$ = stmt;
}
;

statement:
expression ';' {
	STMT(STMT_EXPR);
	stmt->expr.expr = $1;
	$$ = stmt;
}
| let_stmt ';' {
	$$ = $1;
}
| print_stmt ';' {
	$$ = $1;
}
;

program:
statement {
	//print_stmt($1);
	sb_push(*program_stmts, $1);
}
| program statement {
	//print_stmt($2);
	sb_push(*program_stmts, $2);
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
	char * str = malloc(strlen(buffer + 1));
	strcpy(str, buffer);
	return str;
}

void print_stmt(Stmt * stmt)
{
	char * str = str_stmt(stmt);
	printf("%s\n", str);
	free(str);
}

Stmt ** parse()
{
	yydebug = 0;
	init();
	Stmt ** program_stmts = NULL;
	if (yyparse(&program_stmts)) {
		fprintf(stderr, "Encountered error while parsing. Exiting.\n");
		exit(1);
	}
	return program_stmts;
}

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

void yyerror(Stmt *** program_stmts, char const * s)
{
	fprintf(stderr, "%s\n", s);
}
