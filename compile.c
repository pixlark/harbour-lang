#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>
//#include <llvm-c/ExecutionEngine.h>
//#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
//#include <llvm-c/BitWriter.h>

#include "parser.tab.h"
#include "stretchy_buffer.h"

int main()
{
	Stmt ** stmts = parse();
	for (int i = 0; i < sb_count(stmts); i++) {
		//print_stmt(stmts[i]);
	}
	
	LLVMModuleRef module = LLVMModuleCreateWithName("harbour_module");
	
	LLVMValueRef _main;
	{
		LLVMTypeRef main_type = LLVMFunctionType(
			LLVMInt32Type(), NULL, 0, 0);
		_main = LLVMAddFunction(module, "main", main_type);
	}
	
	LLVMBasicBlockRef main_block = LLVMAppendBasicBlock(_main, "main_block");
	
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMPositionBuilderAtEnd(builder, main_block);

	// %x = alloca i32
	LLVMValueRef x = LLVMBuildAlloca(builder, LLVMInt32Type(), "x");
	// store i32 15, i32* %x
	LLVMValueRef store = LLVMBuildStore(
		builder, LLVMConstInt(LLVMInt32Type(), 15, 0), x);
	// %lx = load i32, i32* %x
	LLVMValueRef lx = LLVMBuildLoad(builder, x, "lx");
	// ret i32 %lx
	LLVMValueRef ret = LLVMBuildRet(builder, lx);

	char * error = NULL;
	LLVMVerifyModule(module, LLVMAbortProcessAction, &error);
	LLVMDisposeMessage(error);

	LLVMWriteBitcodeToFile(module, "source.bc");
}
