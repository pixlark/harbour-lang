#pragma once

#include <string.h>
#include "stretchy_buffer.h"

typedef struct Str_Intern {
	size_t len;
	const char * str;
} Str_Intern;

extern Str_Intern * str_interns;
const char * str_intern_range(const char * start, const char * end);
const char * str_intern(const char * str);
