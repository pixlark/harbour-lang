#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void fatal(const char * str, ...)
{
	va_list args;
	va_start(args, str);
	// Print error
	fprintf(stderr, "FATAL ERROR:\n\t", str);
	vfprintf(stderr, str, args);
	fprintf(stderr, "\n");
	// Exit with error code
	va_end(args);
	exit(1);
}
