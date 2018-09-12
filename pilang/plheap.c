#include "plheap.h"

#include "clist.h"
#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static plobj_t **heap;
static size_t heap_cap;
static size_t heap_usage;

static void destroy_list_object(plobj_t *obj);
static void destroy_object(plobj_t *obj);

static void destroy_list_object(plobj_t *obj) {
  list_t list = obj->value.lsvalue;
  for (iter_t it = list_begin(&list);
       !iter_eq(it, list_end(&list));
       it = iter_next(it)) {
    destroy_object((plobj_t*)iter_deref(it));
  }
  destroy_list(&list);
}

static void destroy_object(plobj_t *obj) {
  switch(obj->oid) {
  default:
  
  case OID_INT:
  case OID_FLOAT:
  case OID_STR:
    break;

  case OID_LIST:
    destroy_list_object(obj);
    break;
  
// TODO: do this after we have a set implementation
//  case OID_SET:
//    destroy_set_object(obj);
//    break;

// TODO: do this after we have a struct implementation
//  case OID_STRUCT:
//    destroy_struct_object(obj);
//    break;
  }
}

void init_heap() {
  heap = NEWN(plobj_t*, HEAP_INIT_SIZE);
  for (int i = 0; i < HEAP_INIT_SIZE; i++) {
    heap[i] = NEW(plobj_t);
    heap[i]->used = 0;
  }
  heap_cap = HEAP_INIT_SIZE;
  heap_usage = 0;
}

static void expand_heap(void) {
  size_t new_cap = heap_cap * 1.6;
  plobj_t **new_heap = NEWN(plobj_t*, new_cap);
  for (int i = 0; i < heap_cap; i++) {
    new_heap[i] = heap[i];
  }
  for (int i = heap_cap; i < new_cap; i++) {
    new_heap[i] = NEW(plobj_t);
    new_heap[i]->used = 0;
  }
  free(heap);
  heap = new_heap;
  heap_cap = new_cap;
}

static plobj_t *plalloc(void) {
  if (heap_cap == heap_usage) {
    // TODO gc should be implemented by stack side code
    // heap_request_gc();
  }
  
  if (0.8 * heap_cap < heap_usage) {
    expand_heap();
  }
  
  for (int i = 0; i < heap_cap; i++) {
    if (heap[i]->used == 0) {
      ++heap_usage;
      heap[i]->used = 1;
      return heap[i];
    }
  }
  
  assert(0);
}

void gc_start() {
  for (int i = 0; i < heap_cap; i++) {
    if (heap[i]->used) {
      heap[i]->gcmark = GCM_BLACK;
    }
  }
}

static void gc_mark_list(plobj_t *obj) {
  list_t list = obj->value.lsvalue;
  for (iter_t it = list_begin(&list);
       !iter_eq(it, list_end(&list));
       it = iter_next(it)) {
    gc_mark_white((plobj_t*)iter_deref(it));
  }
}

static void gc_mark_ref(plobj_t *obj) {
  gc_mark_white((plobj_t*)obj->value.pvalue);
}

void gc_mark_white(plobj_t *obj) {
  obj->gcmark = GCM_WHITE;
  switch (obj->oid) {
  case OID_STRUCT:
    // TODO struct marking stuffs
    break;
  
  case OID_REF:
    gc_mark_ref(obj);
    break;
    
  case OID_LIST:
    gc_mark_list(obj);
    break;
  
  default:
    ;
  }
}

void gc_cleanup() {
  for (int i = 0; i < heap_cap; i++) {
    if ((heap[i]->used) && (heap[i]->gcmark == GCM_BLACK)) {
      heap[i]->used = 0;
      destroy_object(heap[i]);
    }
  }
}
