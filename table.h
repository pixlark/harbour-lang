#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "types.h"

Symbol_Table * table_create(size_t size);
void table_add_symbol(Symbol_Table * table, char * key, Symbol value);
bool table_symbol_exists(Symbol_Table * table, char * key);
Symbol table_get_symbol(Symbol_Table * table, char * key);

uintptr_t table_hash(char* key, size_t size);
