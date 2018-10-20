#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	char ** keys;
	int  *  values;
	bool *  taken;
	size_t  size;
} Symbol_Table;

Symbol_Table * table_create(size_t size);
void table_add_symbol(Symbol_Table * table, char * key, int value);
bool table_symbol_exists(Symbol_Table * table, char * key);
int table_get_symbol(Symbol_Table * table, char * key);

uintptr_t table_hash(char* key, size_t size);
