make:
	bison -d parser.y
	flex scanner.l
	gcc -c \
		-DYYDEBUG=1 \
			parser.tab.c \
			lex.yy.c \
			stretchy_buffer.c \
			compile.c \
		$(shell llvm-config --cflags)
	g++ *.o \
		$(shell llvm-config --cxxflags --ldflags --libs --system-libs) \
		-o harbour
	rm *.o *.dwo
