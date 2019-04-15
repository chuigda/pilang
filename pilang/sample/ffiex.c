#include "eval.h"

static int64_t int_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.ivalue : 0;
}

plvalue_t ffi_add(list_t args) {
  int sum = 0;
  for (iter_t it = list_begin(&args);
       !iter_eq(it, list_end(&args));
       it = iter_next(it)) {
    plvalue_t *value = (plvalue_t*)iter_deref(it);
    sum += int_failsafe(fetch_int(*value));
  }
  
  plvalue_t ret = create_temp();
  ret.type = JT_INT;
  ret.value.ivalue = sum;
  return ret;
}
