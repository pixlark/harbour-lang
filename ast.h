#pragma once

#include "types.h"
#include "table.h"

void create_symbols(Function * func);
bool typecheck_expr(Function * func, Expr * expr, Type expected);
bool typecheck_stmt(Function * func, Stmt * stmt);
