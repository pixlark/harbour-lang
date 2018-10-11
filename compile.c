#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <llvm-c/Core.h>
#include <llvm-c/Transforms/PassManagerBuilder.h>
//#include <llvm-c/ExecutionEngine.h>
//#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
//#include <llvm-c/BitWriter.h>

#include "parser.tab.h"
#include "stretchy_buffer.h"
#include "map.h"

// Is type immediately evaluatable? i.e. not a compound expression.
bool is_imval(Expr * expr)
{
	//return type == EXPR_ATOM || type == EXPR_VAR;
	return expr->type == EXPR_ATOM;
}

typedef struct {
	LLVMBuilderRef builder;
	Map * symbol_table;
} Compile_Context;

LLVMValueRef compile_expression(Compile_Context ctx, Expr * expr)
{
	switch (expr->type) {
	case EXPR_ATOM:
		return LLVMConstInt(LLVMInt32Type(), expr->atom.val, 0);
		break;
	case EXPR_VAR: {
		// %lx = load i32, i32* %x
		//LLVMValueRef lx = LLVMBuildLoad(ctx.builder, x, "lx");
		LLVMValueRef var;
		map_index(ctx.symbol_table, expr->var.name, &var);
		return LLVMBuildLoad(ctx.builder, var, "");
	} break;
	case EXPR_BINARY: {
		LLVMValueRef left = compile_expression(ctx, expr->binary.left);
		LLVMValueRef right = compile_expression(ctx, expr->binary.right);
		switch (expr->binary.operator) {
		case OP_ADD:
			return LLVMBuildAdd(ctx.builder, left, right, "");
		case OP_SUB:
			return LLVMBuildSub(ctx.builder, left, right, "");
		case OP_MUL:
			return LLVMBuildMul(ctx.builder, left, right, "");
		case OP_DIV:
			return LLVMBuildUDiv(ctx.builder, left, right, "");
		}
		/*
		  printf("EXPR_BINARY\n");
		  LLVMBuildAdd(
		  ctx.builder,
		  LLVMConstInt(LLVMInt32Type(), expr->binary.left->atom.val,  0),
		  LLVMConstInt(LLVMInt32Type(), expr->binary.right->atom.val, 0),
		  "binary");*/
	} break;
	default:
		fprintf(stderr, "Not supported\n");
		exit(1);
		break;
	}
}

LLVMValueRef compile_statement(Compile_Context ctx, Stmt * stmt)
{
	switch (stmt->type) {
	case STMT_EXPR: {
		return compile_expression(ctx, stmt->expr.expr);
	} break;
	case STMT_LET: {
		LLVMValueRef val = compile_expression(ctx, stmt->let.expr);
		LLVMValueRef var = LLVMBuildAlloca(ctx.builder, LLVMInt32Type(), stmt->let.name);
		LLVMBuildStore(ctx.builder, val, var);
		map_insert(ctx.symbol_table, stmt->let.name, var);
		return var;
	} break;
	default:
		fprintf(stderr, "Not supported\n");
		exit(1);
		break;
	}
}

int main()
{
	Stmt ** stmts = parse();
	for (int i = 0; i < sb_count(stmts); i++) {
		print_stmt(stmts[i]);
	}

	LLVMPassManagerBuilderRef pass_builder = LLVMPassManagerBuilderCreate();
	LLVMPassManagerBuilderSetOptLevel(pass_builder, 0);
	LLVMModuleRef module = LLVMModuleCreateWithName("harbour_module");
	
	LLVMValueRef _main;
	{
		LLVMTypeRef main_type = LLVMFunctionType(
			LLVMInt32Type(), NULL, 0, 0);
		_main = LLVMAddFunction(module, "main", main_type);
	}
	
	LLVMBasicBlockRef main_block = LLVMAppendBasicBlock(_main, "main_block");

	Compile_Context ctx;
	ctx.builder = LLVMCreateBuilder();
	LLVMPositionBuilderAtEnd(ctx.builder, main_block);
	
	ctx.symbol_table = make_map(512);
	
	for (int s = 0; s < sb_count(stmts); s++) {
		Stmt * stmt = stmts[s];
		compile_statement(ctx, stmt);
	}
	
	LLVMBuildRet(ctx.builder, LLVMConstInt(LLVMInt32Type(), 0, 0));
	//LLVMBuildRet(builder, result);
	
	/*
	// %x = alloca i32
	LLVMValueRef x = LLVMBuildAlloca(builder, LLVMInt32Type(), "x");
	// store i32 15, i32* %x
	LLVMValueRef store = LLVMBuildStore(
		builder, LLVMConstInt(LLVMInt32Type(), 15, 0), x);
	// %lx = load i32, i32* %x
	LLVMValueRef lx = LLVMBuildLoad(builder, x, "lx");
	// ret i32 %lx
	LLVMValueRef ret = LLVMBuildRet(builder, lx);*/
	
	char * error = NULL;
	LLVMVerifyModule(module, LLVMAbortProcessAction, &error);
	LLVMDisposeMessage(error);

	LLVMPrintModuleToFile(module, "source.ll", NULL);
	LLVMWriteBitcodeToFile(module, "source.bc");
}
