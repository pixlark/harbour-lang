#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "stretchy_buffer.h"
#include "map.h"

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
			int32_t val;
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

typedef struct Function {
	Stmt ** stmts;
	Map * symbols;
	int stack_offset;
} Function;

void print_expr(Expr * expr);
void print_stmt(Stmt * stmt);
