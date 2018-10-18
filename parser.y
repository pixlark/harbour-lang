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
	#include "types.h"
	#define EXPR(t) \
		Expr * expr = malloc(sizeof(Expr)); \
		expr->type = t;
	#define STMT(t) \
		Stmt * stmt = malloc(sizeof(Stmt)); \
		stmt->type = t;
	Function * parse();
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

Function * parse()
{
	yydebug = 0;
	init();
	Function * func_main = malloc(sizeof(Function));
	func_main->stmts = NULL;
	if (yyparse(&func_main->stmts)) {
		fprintf(stderr, "Encountered error while parsing. Exiting.\n");
		exit(1);
	}
	return func_main;
}

void yyerror(Stmt *** program_stmts, char const * s)
{
	fprintf(stderr, "%s\n", s);
}
