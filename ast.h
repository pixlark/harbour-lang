#pragma once

#include "types.h"
#include "table.h"

void create_symbols(Function * func);
Type typecheck_expr(Function * func, Expr * expr);
Type typecheck_stmt(Function * func, Stmt * stmt);
