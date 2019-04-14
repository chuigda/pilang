#ifndef VALUE_H
#define VALUE_H

#include "stack.h"
#include "heap.h"

typedef enum {
  ROC_TEMP,
  ROC_ONSTACK,
  ROC_ONHEAP,
  ROC_NONE
} plvalue_storage_t;

typedef struct {
  jjvalue_t value;
  int16_t roc;
  int16_t type;
} plvalue_t;

plvalue_t create_onstack(stkobj_t *storage);
plvalue_t create_onheap(heapobj_t *storage);
plvalue_t create_temp();
jjvalue_t *fetch_storage(plvalue_t *obj);
result_t fetch_int(plvalue_t obj);
result_t fetch_float(plvalue_t obj);
result_t fetch_bool(plvalue_t obj);
result_t fetch_str(plvalue_t obj);
result_t fetch_list(plvalue_t obj);
plvalue_t auto_deref(plvalue_t maybe_ref);

typedef struct {
  // heap functions
  // @todo should we make heap replacable instead of singleton?
  // okay, maybe that's unnecessary, but at least we must make heap
  // "first class"
  heapobj_t* (*heap_alloc_int_fn)(int64_t);
  heapobj_t* (*heap_alloc_bool_fn)(bool);
  heapobj_t* (*heap_alloc_float_fn)(double);
  heapobj_t* (*heap_alloc_str_fn)(strhdl_t);
  heapobj_t* (*heap_alloc_list_fn)(list_t);
  heapobj_t* (*heap_alloc_handle_fn)(res_base_t*);
  heapobj_t* (*heap_alloc_empty_fn)();
  
  // stack
  stack_t *stack;
  
  // string operations
  strhdl_t (*create_string_fn)(const char*);
  const char* (*get_string_fn)(strhdl_t);
} host_env_t;

host_env_t get_host_env();

#endif // VALUE_H
