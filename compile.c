#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "ast.h"
#include "error.h"
#include "parser.tab.h"
#include "stretchy_buffer.h"
#include "table.h"
#include "types.h"

static FILE * out_file;

typedef enum {
	REG_0 = 0, 
	REG_1,  REG_2,  REG_3,
	REG_4,  REG_5,  REG_6,
	REG_7,  REG_8,  REG_9,
	REG_10, REG_11, REG_12,
} Reg;

#define EMIT_NOTES 0
#if EMIT_NOTES
	#define NOTE(str, ...) \
		fprintf(out_file, "@" str "\n", ##__VA_ARGS__);
#else
	#define NOTE(str, ...)
#endif

#if 0
#define INVALIDATE_OUTPUT 0
void fatal(const char * str, ...)
{
	va_list args;
	va_start(args, str);
	// Print error
	fprintf(stderr, "FATAL ERROR:\n\t", str);
	vfprintf(stderr, str, args);
	fprintf(stderr, "\n");
	#if INVALIDATE_OUTPUT
	// Invalidate output file
	int len = ftell(out_file);
	len = len + sizeof(uint32_t) - (len % sizeof(uint32_t));
	char * buf = malloc(len);
	for (int i = 0; i < len / sizeof(uint32_t); i++) {
		((uint32_t*) buf)[i] = 0xEFBEADDE; // Little-endian
	}
	fseek(out_file, 0, SEEK_SET);
	fwrite(buf, 1, len, out_file);
	#endif
	// Close output file
	fclose(out_file);
	// Exit with error code
	va_end(args);
	exit(1);	
}
#endif

void emit_header()
{
	NOTE("emit_header");
	fprintf(out_file, ".arch armv4t\n"
					  ".global main\n"
					  ".text\n"
					  ".arm\n");
}

void emit_label(char * label)
{
	NOTE("emit_label");
	fprintf(out_file, "%s:\n", label);
}

void emit_func_start(Function * func)
{
	NOTE("emit_func_start");
	fprintf(out_file,
		"push {fp, lr}\n"
		"mov fp, sp\n"
		"sub  sp, #%u\n", func->stack_offset);
}

void emit_func_end(Function * func)
{
	NOTE("emit_func_end");
	fprintf(out_file,
		"add sp, #%u\n"
		"pop {fp, lr}\n"
		"bx  lr\n", func->stack_offset);
}

void emit_push(Reg reg)
{
	NOTE("emit_push");
	fprintf(out_file, "push {r%u}\n", reg);
}

void emit_push_i32(int32_t x)
{
	NOTE("emit_push_i32");
	fprintf(out_file, "mov r12, #%d\n", x);
	emit_push(REG_12);
}

void emit_pop(Reg reg)
{
	NOTE("emit_pop");
	fprintf(out_file, "pop {r%u}\n", reg);
}

void emit_bin_op(Operator op, Reg self, Reg other)
{
	NOTE("emit_op");
	switch (op) {
	case OP_ADD:
		fprintf(out_file, "add r%u, r%u\n", self, other);
		break;
	case OP_SUB:
		fprintf(out_file, "sub r%u, r%u\n", self, other);
		break;
	case OP_MUL:
		fprintf(out_file, "mul r%u, r%u\n", self, other);
		break;
	case OP_DIV:
		fprintf(out_file, "push {r%u, r%u}\n"
						  "pop  {r0, r1}\n"
						  "bl __aeabi_idiv\n", self, other);
		break;
	default:
		fatal("Unsupported operation");
		break;
	}	
}

void emit_unary_op(Operator op, Reg reg)
{
	NOTE("emit_unary_op");
	switch (op) {
	case OP_NEG:
		fprintf(out_file, "neg r%u, r%u\n", reg, reg);
		break;
	default:
		fatal("Unsupported operation");
		break;
	}
}

void emit_stack_load(Reg reg, int offset)
{
	NOTE("emit_stack_load");
	fprintf(out_file,
		"ldr r%u, [fp, #%d]\n",
		reg, offset);
}

void emit_stack_save(Reg reg, int offset)
{
	NOTE("emit_stack_save");
	fprintf(out_file,
		"str r%u, [fp, #%d]\n",
		reg, offset);
}

int var_offset(Function * func, const char * name)
{
	if (!table_symbol_exists(func->symbols, name)) {
		fatal("Nonexistent symbol '%s'", name);
	}
	Symbol sym = table_get_symbol(func->symbols, name);
	return sym.offset;
}

void compile_expression(Function * func, Expr * expr)
{
	switch (expr->type) {
	case EXPR_ATOM: {
		//emit_push_i32(expr->atom.val);
	} break;
	case EXPR_VAR: {
		size_t offset = var_offset(func, expr->var.name);
		emit_stack_load(REG_0, offset);
		emit_push(REG_0);
	} break;
	case EXPR_UNARY: {
		compile_expression(func, expr->unary.operand);
		emit_pop(REG_0);
		emit_unary_op(expr->unary.operator, REG_0);
		emit_push(REG_0);
	} break;
	case EXPR_BINARY: {
		compile_expression(func, expr->binary.left);
		compile_expression(func, expr->binary.right);
		emit_pop(REG_1); // right value
		emit_pop(REG_0); // left value
		emit_bin_op(expr->binary.operator, REG_0, REG_1);
		emit_push(REG_0);
	} break;
	}
}

void compile_statement(Function * func, Stmt * stmt)
{
	switch (stmt->type) {
	case STMT_EXPR:
		compile_expression(func, stmt->expr.expr);
		emit_pop(REG_0);
		break;
	case STMT_LET: {
		compile_expression(func, stmt->let.expr);
		emit_pop(REG_0);
		size_t offset = var_offset(func, stmt->let.name);
		emit_stack_save(REG_0, offset);
	} break;
	case STMT_ASSIGN: {
		compile_expression(func, stmt->assign.expr);
		emit_pop(REG_0);
		size_t offset = var_offset(func, stmt->assign.name);
		emit_stack_save(REG_0, offset);
	} break;
	case STMT_PRINT: {
		compile_expression(func, stmt->print.expr);
		emit_pop(REG_1);
		fprintf(out_file, "ldr r0, =D__fmt_string\n"
						  "bl printf\n");
	} break;
	default:
		fatal("Statement not supported");
		break;
	}
}

int main()
{
	Function * func_main = parse();

	// Fill our symbol table
	create_symbols(func_main);

	// Typecheck main
	for (int i = 0; i < sb_count(func_main->stmts); i++) {
		print_stmt(func_main->stmts[i]);
		typecheck_stmt(func_main, func_main->stmts[i]);
	}

	return 0;
	
	// Output assembly
	out_file = fopen("out.s", "w");
	
	emit_header();
	emit_label("main");

	emit_func_start(func_main);

	for (int i = 0; i < sb_count(func_main->stmts); i++) {
		compile_statement(func_main, func_main->stmts[i]);
	}

	emit_func_end(func_main);

	fprintf(out_file, ".data\n"
					  "D__fmt_string:\n"
					  ".ascii \"%s\\n\\0\"\n", "%d");

	fclose(out_file);

	return 0;
}
