#include "ast.h"

void print_symbols(Map * map)
{
	printf("-- SYMBOLS --\n");
	for (int i = 0; i < map->size; i++) {
		if (map->taken[i]) {
			printf("'%s' (@%p) -> %d\n",
				(char*) map->keys[i],
				(char*) map->keys[i],
				(int) map->values[i]);
		}
	}
	printf("-------------\n");
}

void tag_stmt(Function * func, Stmt * stmt)
{
	switch (stmt->type) {
	case STMT_LET:
		if (!map_index(
			func->symbols,
			(uint64_t) stmt->let.name, NULL)) {
			map_insert(
				func->symbols,
				(uint64_t) stmt->let.name,
				(uint64_t) func->stack_offset);
			func->stack_offset += sizeof(int32_t);
		}
		break;
	default:
		break;
	}
}

void tag_function_vars(Function * func)
{
	func->symbols = make_map(512);
	func->stack_offset = 0;

	// Tag initial offsets
	for (int i = 0; i < sb_count(func->stmts); i++) {
		tag_stmt(func, func->stmts[i]);
	}

	// Retag as offset from FP rather than SP
	for (int i = 0; i < func->symbols->size; i++) {
		if (func->symbols->taken[i]) {
			//((int) func->symbols->values[i]) -= func->stack_offset;
			func->symbols->values[i] = (uint64_t) (((int) func->symbols->values[i]) - func->stack_offset);
		}
	}
	
	printf("Frame size: %d\n", func->stack_offset);
	print_symbols(func->symbols);
}
