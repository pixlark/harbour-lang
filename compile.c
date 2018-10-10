#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>
//#include <llvm-c/ExecutionEngine.h>
//#include <llvm-c/Target.h>
//#include <llvm-c/Analysis.h>
//#include <llvm-c/BitWriter.h>

#include "parser.tab.h"
#include "stretchy_buffer.h"

int main()
{
	Stmt ** stmts = parse();
	for (int i = 0; i < sb_count(stmts); i++) {
		print_stmt(stmts[i]);
	}
	
	LLVMModuleRef module = LLVMModuleCreateWithName("harbour_module");
}
