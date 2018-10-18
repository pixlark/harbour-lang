.PHONY: c harbour test

c:
	bison -d parser.y
	flex scanner.l
	cd obj && \
	gcc -g -c \
		-DYYDEBUG=1 \
			../parser.tab.c \
			../lex.yy.c \
			../stretchy_buffer.c \
			../compile.c \
			../map.c \
		$(shell llvm-config --cflags)
	g++ -g obj/*.o \
		$(shell llvm-config --cxxflags --ldflags --libs --system-libs) \
		-o harbour

harbour:
	cat source.hb | ./harbour
	llc -filetype=obj source.bc
	clang source.o -o source
	rm -f source.o

test:
	llc -filetype=obj test.ll
	clang test.o -o test
	rm -f test.o

gcc=arm-linux-gnueabi-gcc
as=arm-linux-gnueabi-as
ld=arm-linux-gnueabi-ld

arm:
	$(as) out.s -as -mfloat-abi=soft -mcpu=all -march=armv4 -o out.o
	$(gcc) -static out.o -o out
