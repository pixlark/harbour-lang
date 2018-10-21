#pragma once

#include "types.h"
#include "table.h"

void create_symbols(Function * func);
bool typecheck_expr(Expr * expr, Type expected);
