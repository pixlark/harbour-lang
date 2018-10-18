#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "parser.tab.h"
#include "stretchy_buffer.h"
#include "map.h"

static FILE * out_file;

typedef enum {
	REG_0 = 0, 
	REG_1,  REG_2,  REG_3,
	REG_4,  REG_5,  REG_6,
	REG_7,  REG_8,  REG_9,
	REG_10, REG_11, REG_12,
} Reg;

#define EMIT_NOTES 1
#if EMIT_NOTES
	#define NOTE(str, ...) \
		fprintf(out_file, "@" str "\n", ##__VA_ARGS__);
#else
	#define NOTE(str, ...)
#endif

#define INVALIDATE_OUTPUT 0
void fatal(const char * str)
{
	// Print error
	fprintf(stderr, "FATAL ERROR:\n\t%s\n", str);
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
	exit(1);	
}

void emit_header()
{
	NOTE("emit_header");
	fprintf(out_file, ".arch armv4t\n"
					  ".global main\n"
					  ".text\n"
					  ".arm\n");
}

char * load_string_from_file(char * path)
{
	FILE * file = fopen(path, "r");
	if (file == NULL) return NULL;
	int file_len = 0;
	while (fgetc(file) != EOF) file_len++;
	char * str = (char*) malloc(file_len + 1);
	str[file_len] = '\0';
	fseek(file, 0, SEEK_SET);
	for (int i = 0; i < file_len; i++) str[i] = fgetc(file);
	fclose(file);
	return str;
}

void emit_procedure(char * path)
{
	NOTE("IMPORTED PROCEDURE from '%s'", path);
	char * proc_code = load_string_from_file(path);
	fprintf(out_file, "%s\n", proc_code);
	free(proc_code);
	NOTE("END IMPORTED PROCEDURE");
}

void emit_label(char * label)
{
	NOTE("emit_label");
	fprintf(out_file, "%s:\n", label);
}

void emit_func_save()
{
	NOTE("emit_func_save");
	fprintf(out_file, "push {fp, lr}\n"
					  "add  fp, sp, #4\n");
}

void emit_func_load()
{
	NOTE("emit_func_load");
	fprintf(out_file, "pop {fp, lr}\n"
					  "bx  lr\n");
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
	char * op_str;
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
		
		break;
	default:
		fatal("Unsupported operation");
		break;
	}
}

void compile_expression(Expr * expr)
{
	switch (expr->type) {
	case EXPR_ATOM: {
		emit_push_i32(expr->atom.val);
	} break;
	case EXPR_UNARY: {
		compile_expression(expr->unary.expr);
		emit_pop(REG_0);
		emit_unary_op(expr->unary.operator, REG_0);
	} break;
	case EXPR_BINARY: {
		compile_expression(expr->binary.left);
		compile_expression(expr->binary.right);
		emit_pop(REG_1); // right value
		emit_pop(REG_0); // left value
		emit_bin_op(expr->binary.operator, REG_0, REG_1);
		emit_push(REG_0);
	} break;
	}
}

int main()
{
	Stmt ** stmts = parse();
	for (int i = 0; i < sb_count(stmts); i++) {
		print_stmt(stmts[i]);
	}
	out_file = fopen("out.s", "w");

	emit_header();
	emit_label("main");

	emit_func_save();

	for (int i = 0; i < sb_count(stmts); i++) {
		if (stmts[i]->type == STMT_EXPR) {
			compile_expression(stmts[i]->expr.expr);
			emit_pop(REG_0);
		}
	}

	emit_func_load();

	fclose(out_file);
	return 0;	
}
