#include "eval.h"
#include <stdlib.h>

const char* *describe_ffi_funcs(void) {
  static const char* funcs[] = {
    "ffi_nmsl",
    "ffi_hjyz",
    "ffi_cnmb",
    NULL
  };
  return funcs;
}

const char* *describe_ffi_aliases(void) {
  static const char* aliases[] = {
    "nmsl",
    "hjyz",
    "cnmb",
    NULL
  };
  return aliases;
}

plvalue_t ffi_nmsl(list_t args) {
  (void)args;
  plvalue_t ret = create_temp();
  ret.type = JT_STR;
  ret.value.svalue = create_string("nmsl");
  return ret;
}

plvalue_t ffi_hjyz(list_t args) {
  (void)args;
  return create_onheap(heap_alloc_int(7777));
}

typedef struct {
  RESOURCE_COMMON_HEAD
  char *buffer;
} cnm_t;

static void destroy_cnm(void* res) {
  cnm_t *cnm = (cnm_t*)res;
  free(cnm->buffer);
  free(cnm);
}

plvalue_t ffi_cnmb(list_t args) {
  (void)args;
  cnm_t *cnm = NEW(cnm_t);
  cnm->buffer = NEWN(char, 1024);
  cnm->destructor = &destroy_cnm;
  
  heapobj_t *obj = heap_alloc_handle((res_base_t*)cnm);
  plvalue_t ret = create_temp();
  ret.type = JT_REF;
  ret.value.pvalue = obj;
  return ret;
}
