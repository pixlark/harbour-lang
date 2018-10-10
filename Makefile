make:
	bison -d parser.y
	flex scanner.l
	gcc -DYYDEBUG=1 parser.tab.c lex.yy.c stretchy_buffer.c -o harbour
