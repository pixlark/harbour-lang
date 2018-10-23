#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stretchy_buffer.h"

typedef enum {
	TYPE_BOOL = 1 << 0,
	TYPE_I32  = 1 << 1,
	TYPE_I16  = 1 << 2,
	TYPE_I8   = 1 << 3,
	TYPE_U32  = 1 << 4,
	TYPE_U16  = 1 << 5,
	TYPE_U8   = 1 << 6,
} Type;
extern const char * type_str[];
typedef enum {
	OP_NEG,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
} Operator;
extern const uint32_t operator_types[];
extern const char * operator_str[];
typedef enum {
	EXPR_ATOM,
	EXPR_VAR,
	EXPR_UNARY,
	EXPR_BINARY,
	EXPR_FUNCALL,
} Expr_Type;
extern const char * expr_str[];
typedef struct Expr {
	Expr_Type type;
	union {
		struct {
			Type val_type;
			union {
				int32_t _i32;
				uint8_t _bool;
			};
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
	STMT_ASSIGN,
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
			const char * name;
			Expr * expr;
		} assign;
		struct {
			Expr * expr;
		} print;
	};
} Stmt;

typedef struct {
	int offset;
	Type type;
} Symbol;

typedef struct {
	char ** keys;
	Symbol * values;
	bool * taken;
	size_t size;
} Symbol_Table;

typedef struct Function {
	Stmt ** stmts;
	Symbol_Table * symbols;
	int stack_offset;
} Function;

void print_expr(Expr * expr);
void print_stmt(Stmt * stmt);

Symbol create_symbol(Type type, int offset);

Symbol get_symbol(Function * function, char * name);
