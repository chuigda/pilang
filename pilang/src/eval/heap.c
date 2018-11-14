#include "heap.h"

#include "clist.h"
#include "config.h"
#include "util.h"

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

heapobj_id_t jt2hoid(jjtype_t jt) {
  assert(jt != JT_REF);
  return (heapobj_id_t)jt;
}

jjtype_t hoid2jt(heapobj_id_t hoid) {
  return (jjtype_t)hoid;
}

static heapobj_t **heap;
static size_t heap_cap;
static size_t heap_usage;

void destroy_object(heapobj_t *obj) {
  switch(obj->oid) {
  default:

  case HOID_INT:
  case HOID_FLOAT:
  case HOID_STR:
    break;

  case HOID_LIST:
    destroy_list(&(obj->value.lsvalue));
    break;
  }
}

void init_heap() {
  heap = NEWN(heapobj_t*, PLI_HEAP_INIT_SIZE);
  for (int i = 0; i < PLI_HEAP_INIT_SIZE; i++) {
    heap[i] = NEW(heapobj_t);
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
  heapobj_t **new_heap = NEWN(heapobj_t*, new_cap);
  for (size_t i = 0; i < heap_cap; i++) {
    new_heap[i] = heap[i];
  }
  for (size_t i = heap_cap; i < new_cap; i++) {
    new_heap[i] = NEW(heapobj_t);
    new_heap[i]->used = 0;
  }
  free(heap);
  heap = new_heap;
  heap_cap = new_cap;
}

static heapobj_t *plalloc(void) {
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

heapobj_t *heap_alloc_int(int64_t value) {
  heapobj_t *ret = plalloc();
  ret->oid = HOID_INT;
  ret->value.ivalue = value;
  return ret;
}

heapobj_t *heap_alloc_float(double value) {
  heapobj_t *ret = plalloc();
  ret->oid = HOID_FLOAT;
  ret->value.fvalue = value;
  return ret;
}

heapobj_t *heap_alloc_bool(bool value) {
  heapobj_t *ret = plalloc();
  ret->oid = HOID_BOOL;
  ret->value.bvalue = value;
  return ret;
}

heapobj_t *heap_alloc_list(list_t list) {
  heapobj_t *ret = plalloc();
  ret->oid = HOID_LIST;
  ret->value.lsvalue = list;
  return ret;
}

heapobj_t *heap_alloc_str(int64_t str) {
  heapobj_t *ret = plalloc();
  ret->oid = HOID_STR;
  ret->value.svalue = str;
  return ret;
}

heapobj_t *heap_alloc_empty() {
  heapobj_t *ret = plalloc();
  ret->oid = HOID_UNDEFINED;
  return ret;
}

void gc_start() {
  for (size_t i = 0; i < heap_cap; i++) {
    if (heap[i]->used) {
      heap[i]->gcmark = GCM_BLACK;
    }
  }
}

static void gc_mark_list(heapobj_t *obj) {
  list_t list = obj->value.lsvalue;
  for (iter_t it = list_begin(&list);
       !iter_eq(it, list_end(&list));
       it = iter_next(it)) {
    gc_mark_white((heapobj_t*)iter_deref(it));
  }
}

static void gc_mark_ref(heapobj_t *obj) {
  gc_mark_white((heapobj_t*)obj->value.pvalue);
}

void gc_mark_white(heapobj_t *obj) {
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
