#include "builtins.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

static plvalue_t builtin_print(list_t args) {
  for (iter_t it = list_begin(&args);
       !iter_eq(it, list_end(&args));
       it = iter_next(it)) {
    plvalue_t *value = (plvalue_t*)iter_deref(it);
    *value = auto_deref(*value);
    jjvalue_t *storage = fetch_storage(value);

    switch (value->type) {
    case JT_INT:
      printf("%" PRId64, storage->ivalue); break;
    case JT_FLOAT:
      printf("%f", storage->fvalue); break;
    case JT_BOOL:
      printf("%s", storage->bvalue ? "true" : "false");
      break;
    case JT_STR:
      printf("%s", get_string(storage->svalue)); break;
    case JT_LIST:
      printf("(list)"); break;
    case JT_UNDEFINED:
      printf("Undefined"); break;
    default:
      printf("error");
    }
  }
  plvalue_t ret = create_temp();
  ret.type = JT_INT;
  ret.value.ivalue = list_size(&args);
  return ret;
}

static plvalue_t builtin_readint(list_t args) {
  (void)args;
  plvalue_t ret = create_temp();
  ret.type = JT_INT;
  scanf("%" PRId64, &(ret.value.ivalue));
  return ret;
}

static plvalue_t builtin_readfloat(list_t args) {
  (void)args;
  plvalue_t ret = create_temp();
  ret.type = JT_FLOAT;
  scanf("%lf", &(ret.value.fvalue));
  return ret;
}

static plvalue_t builtin_readstr(list_t args) {
  (void)args;
  static char buffer[4096];
  plvalue_t ret = create_temp();
  ret.type = JT_STR;
  scanf("%s", buffer);
  ret.value.svalue = create_string(buffer);
  return ret;
}

static plvalue_t builtin_copy_to_heap(list_t args) {
  if (list_size(&args) < 1) {
    eprintf0("e: toheap requires one argument\n");
    plvalue_t ret = create_temp();
    ret.type = JT_UNDEFINED;
    return ret;
  }

  if (list_size(&args) > 2) {
    eprintf0("w: toheap require only one argument\n");
  }

  plvalue_t *value = (plvalue_t*)iter_deref(list_begin(&args));
  *value = auto_deref(*value);
  jjvalue_t *storage = fetch_storage(value);

  if (value->roc == ROC_ONHEAP) {
    eprintf0("e: already on heap\n");
    plvalue_t ret = create_temp();
    ret.type = JT_UNDEFINED;
    return ret;
  }

  plvalue_t ref = create_temp();
  ref.type = JT_REF;
  switch (value->type) {
  case JT_INT:
    ref.value.pvalue = heap_alloc_int(storage->ivalue); break;
  case JT_FLOAT:
    ref.value.pvalue = heap_alloc_float(storage->fvalue); break;
  case JT_STR:
    ref.value.pvalue = heap_alloc_str(storage->svalue); break;
  case JT_BOOL:
    ref.value.pvalue = heap_alloc_bool(storage->bvalue); break;
  case JT_UNDEFINED:
    ref.value.pvalue = heap_alloc_empty(); break;
  default: UNREACHABLE
  }
  return ref;
}

plvalue_t builtin_call(strhdl_t name, list_t args) {
  /// @todo replace this with TableGen
  static bool initialized = false;
  static strhdl_t builtin_func_names[11];
  if (!initialized) {
    builtin_func_names[0] = create_string("print");
    builtin_func_names[1] = create_string("readint");
    builtin_func_names[2] = create_string("readfloat");
    builtin_func_names[3] = create_string("readstr");
    builtin_func_names[4] = create_string("new");
    builtin_func_names[5] = create_string("deref");
    builtin_func_names[6] = create_string("createlist");
    builtin_func_names[7] = create_string("pushback");
    builtin_func_names[8] = create_string("popback");
    builtin_func_names[9] = create_string("atput");
    builtin_func_names[10] = create_string("at");
    initialized = true;
  }

  static builtin_func_t builtin_funcs[11] = {
    builtin_print, builtin_readint, builtin_readfloat, builtin_readstr,
    builtin_copy_to_heap, NULL, NULL, NULL, NULL, NULL, NULL
  };

  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;

  for (size_t i = 0; i < COUNTOF(builtin_func_names); i++) {
    if (name == builtin_func_names[i]) {
      if (builtin_funcs[i] == NULL) {
        eprintf("sorry, %s not implemented yet.\n", get_string(name));
        break;
      }
      ret = (builtin_funcs[i])(args);
    }
  }

  return ret;
}
