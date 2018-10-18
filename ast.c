#include "ast.h"

void print_symbols(Map * map)
{
	printf("-- SYMBOLS --\n");
	for (int i = 0; i < map->size; i++) {
		if (map->taken[i]) {
			printf("'%s' (@%p) -> %u\n",
				(char*) map->keys[i],
				(char*) map->keys[i],
				(size_t) map->values[i]);
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
				func->symbols, (uint64_t) stmt->let.name,
				func->stack_offset);
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

	for (int i = 0; i < sb_count(func->stmts); i++) {
		tag_stmt(func, func->stmts[i]);
	}

	printf("Frame size: %u\n", func->stack_offset);
	print_symbols(func->symbols);
}
