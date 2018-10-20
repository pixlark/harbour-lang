#include "table.h"

#include <stdio.h>
#include <assert.h>

Symbol_Table * table_create(size_t size)
{
	Symbol_Table * table = malloc(sizeof(Symbol_Table));
	table->keys   = malloc(sizeof(char*) * size);
	table->values = malloc(sizeof(int) * size);
	table->taken  = calloc(size, sizeof(bool));
	table->size = size;
	return table;
}

void table_add_symbol(Symbol_Table * table, char * key, int value)
{
	int position = table_hash(key, table->size);
	int counter = 0;
	while (table->taken[position]) {
		if (table->keys[position] == key) break;
		assert(counter++ < table->size); // Don't exceed maximum table size
		position = (position + 1) % table->size;
	}
	table->keys[position]   = key;
	table->values[position] = value;
	table->taken[position]  = true;
}

bool table_symbol_exists(Symbol_Table * table, char * key)
{
	for (int i = 0; i < table->size; i++) {
		if (table->taken[i] && table->keys[i] == key) {
			return true;
		}
	}
	return false;
}

int table_get_symbol(Symbol_Table * table, char* key)
{
	assert(table_symbol_exists(table, key));
	int position = table_hash(key, table->size);
	int counter = 0;
	while (1) {
		if (!table->taken[position]) {
			return false;
		} else if (table->keys[position] == key) {
			break;
		}
		if (counter++ >= table->size) return false;
		position = (position + 1) % table->size;
	}
	return table->values[position];
}

uintptr_t table_hash(char * key, size_t size)
{
	return ((uintptr_t) key) % size;
}
