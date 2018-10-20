#pragma once

#include "types.h"

void create_symbols(Function * func);
bool typecheck_expr(Expr * expr, Type expected);
