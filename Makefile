.PHONY: c harbour test

c:
	bison -d parser.y
	flex scanner.l
	cd obj && \
	gcc -std=c99 -g \
		-DYYDEBUG=1 \
			../ast.c \
			../compile.c \
			../error.c \
			../lex.yy.c \
			../parser.tab.c \
			../stretchy_buffer.c \
			../table.c \
			../types.c \
		-o ../harbour


#		$(shell llvm-config --cflags)
#	g++ -g obj/*.o \
#		$(shell llvm-config --cxxflags --ldflags --libs --system-libs) \
#		-o harbour

harbour:
	cat source.hb | ./harbour
	llc -filetype=obj source.bc
	clang source.o -o source
	rm -f source.o

gcc=arm-linux-gnueabi-gcc
as=arm-linux-gnueabi-as
ld=arm-linux-gnueabi-ld

arm:
	$(as) out.s -as -mfloat-abi=soft -mcpu=all -march=armv4 -o out.o
	$(gcc) -static out.o -o out

test:
	$(as) test.s -as -mfloat-abi=soft -mcpu=all -march=armv4 -o test.o
	$(gcc) -static test.o -o test
