#include "value.h"
#include "builtins.h"

static host_env_t host_env;

void setup_host_env(host_env_t env) {
  host_env = env;
}

const char* *describe_ffi_funcs(void) {
  static const char* funcs[] = {
    "ffi_nmsl",
    "ffi_hjyz",
    NULL
  };
  return funcs;
}

const char* *describe_ffi_aliases(void) {
  static const char* aliases[] = {
    "nmsl",
    "hjyz",
    NULL
  };
  return aliases;
}

plvalue_t ffi_nmsl(list_t args) {
  (void)args;
  plvalue_t ret = create_temp();
  ret.type = JT_STR;
  ret.value.svalue = host_env.create_string_fn("nmsl");
  return ret;
}

plvalue_t ffi_hjyz(list_t args) {
  (void)args;
  return create_onheap(host_env.heap_alloc_int_fn(7777));
}
