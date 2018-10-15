.PHONY: c harbour test

c:
	bison -d parser.y
	flex scanner.l
	gcc -g -c \
		-DYYDEBUG=1 \
			parser.tab.c \
			lex.yy.c \
			stretchy_buffer.c \
			compile.c \
			map.c \
		$(shell llvm-config --cflags)
	g++ -g *.o \
		$(shell llvm-config --cxxflags --ldflags --libs --system-libs) \
		-o harbour
	rm -f *.o *.dwo

harbour:
	cat source.hb | ./harbour
	llc -filetype=obj source.bc
	clang source.o -o source
	rm -f source.o

test:
	llc -filetype=obj test.ll
	clang test.o -o test
	rm -f test.o
