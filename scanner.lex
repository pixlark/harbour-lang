%option noyywrap
%{
	#include <stdio.h>
	#include "parser.tab.h"
%}

%%

[_A-Za-z][_A-Za-z0-9]* {
	yylval.ident = str_intern(yytext);
	return IDENT;
}

[0-9]+ {
	yylval.val = atoi(yytext);
	return INT;
}

"("|")"|"*"|"/"|"+"|"-"|"," {
	return *yytext;
}

" "|"\t"|"\r"|"\n" {}

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
