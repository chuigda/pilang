#ifndef BUILTINS_H
#define BUILTINS_H

#include "value.h"
#include "clist.h"

typedef plvalue_t (*builtin_func_t)(list_t);

plvalue_t builtin_call(strhdl_t name, list_t args);

#endif // BUILTINS_H
