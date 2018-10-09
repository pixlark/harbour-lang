%option noyywrap
%{
	#include <stdio.h>
	#include "parser.tab.h"
%}

%%

" "|"\t"|"\r"|"\n" {}

"("|")"|"*"|"/"|"+"|"-"|","|";"|":"|"=" {
	return *yytext;
}

"print" { return PRINT_KW; }
"let" { return LET_KW; }

"i32" {
	yylval.type = TYPE_I32;
	return TYPE;
}

[_A-Za-z][_A-Za-z0-9]* {
	yylval.ident = str_intern(yytext);
	return IDENT;
}

[0-9]+ {
	yylval.val = atoi(yytext);
	return INT_LIT;
}

. {
	fprintf(stderr, "Unrecognized character '%c'\n", *yytext);
	exit(1);
}

%%

void yyerror(char const * s)
{
	fprintf(stderr, "%s\n", s);
}

void init()
{
	yyin = stdin;
	FILE * tmp = tmpfile();
	yyout = tmp;
}
