#include "plheap.h"

#include "clist.h"
#include "config.h"
#include "util.h"

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

plheapobj_id_t jt2hoid(jjtype_t jt) {
  assert(jt != JT_REF);
  return (plheapobj_id_t)jt;
}

jjtype_t hoid2jt(plheapobj_id_t hoid) {
  return (jjtype_t)hoid;
}

static plheapobj_t **heap;
static size_t heap_cap;
static size_t heap_usage;

void destroy_object(plheapobj_t *obj) {
  switch(obj->oid) {
  default:

  case HOID_INT:
  case HOID_FLOAT:
  case HOID_STR:
    break;

  case HOID_LIST:
    destroy_list(&(obj->value.lsvalue));
    break;

// TODO: do this after we have a set implementation
//  case HOID_SET:
//    destroy_set_object(obj);
//    break;

// TODO: do this after we have a struct implementation
//  case HOID_STRUCT:
//    destroy_struct_object(obj);
//    break;
  }
}

void init_heap() {
  heap = NEWN(plheapobj_t*, PLI_HEAP_INIT_SIZE);
  for (int i = 0; i < PLI_HEAP_INIT_SIZE; i++) {
    heap[i] = NEW(plheapobj_t);
    heap[i]->used = 0;
  }
  heap_cap = PLI_HEAP_INIT_SIZE;
  heap_usage = 0;
}

void close_heap() {
  for (size_t i = 0; i < heap_cap; i++) {
    free(heap[i]);
  }
  free(heap);
}

static void expand_heap(void) {
  size_t new_cap = heap_cap * 1.6;
  plheapobj_t **new_heap = NEWN(plheapobj_t*, new_cap);
  for (size_t i = 0; i < heap_cap; i++) {
    new_heap[i] = heap[i];
  }
  for (size_t i = heap_cap; i < new_cap; i++) {
    new_heap[i] = NEW(plheapobj_t);
    new_heap[i]->used = 0;
  }
  free(heap);
  heap = new_heap;
  heap_cap = new_cap;
}

static plheapobj_t *plalloc(void) {
  if (heap_cap == heap_usage) {
    // TODO gc should be implemented by stack side code
    // heap_request_gc();
  }

  if (0.8 * heap_cap < heap_usage) {
    expand_heap();
  }

  for (size_t i = 0; i < heap_cap; i++) {
    if (heap[i]->used == 0) {
      ++heap_usage;
      heap[i]->used = 1;
      return heap[i];
    }
  }

  assert(0);
}

plheapobj_t *heap_alloc_int(int64_t value) {
  plheapobj_t *ret = plalloc();
  ret->oid = HOID_INT;
  ret->value.ivalue = value;
  return ret;
}

plheapobj_t *heap_alloc_float(double value) {
  plheapobj_t *ret = plalloc();  
  ret->oid = HOID_FLOAT;
  ret->value.fvalue = value;
  return ret;
}

plheapobj_t *heap_alloc_list(list_t list) {
  plheapobj_t *ret = plalloc();
  ret->oid = HOID_LIST;
  ret->value.lsvalue = list;
  return ret;
}

plheapobj_t *heap_alloc_str(int64_t str) {
  plheapobj_t *ret = plalloc();
  ret->oid = HOID_STR;
  ret->value.svalue = str;
  return ret;
}

void gc_start() {
  for (size_t i = 0; i < heap_cap; i++) {
    if (heap[i]->used) {
      heap[i]->gcmark = GCM_BLACK;
    }
  }
}

static void gc_mark_list(plheapobj_t *obj) {
  list_t list = obj->value.lsvalue;
  for (iter_t it = list_begin(&list);
       !iter_eq(it, list_end(&list));
       it = iter_next(it)) {
    gc_mark_white((plheapobj_t*)iter_deref(it));
  }
}

static void gc_mark_ref(plheapobj_t *obj) {
  gc_mark_white((plheapobj_t*)obj->value.pvalue);
}

void gc_mark_white(plheapobj_t *obj) {
  obj->gcmark = GCM_WHITE;
  switch (obj->oid) {
  case HOID_LIST:
    gc_mark_list(obj);
    break;
  
  default:
    ;
  }
}

void gc_cleanup() {
  for (size_t i = 0; i < heap_cap; i++) {
    if ((heap[i]->used) && (heap[i]->gcmark == GCM_BLACK)) {
      heap[i]->used = 0;
      destroy_object(heap[i]);
      --heap_usage;
    }
  }
}
