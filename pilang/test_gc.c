#include "plheap.h"

#include "mstring.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  init_heap();
  
  plobj_t *o1 = plobj_create_int(39);
  plobj_t *o2 = plobj_create_float(39.0);
  plobj_t *o3 = plobj_create_str(create_string("fuck"));
  plobj_t *o4 = plobj_create_str(create_string("shit"));
  plobj_t *ro1 = plobj_create_ref(o1);
  plobj_t *ro2 = plobj_create_ref(o2);
  plobj_t *ro3 = plobj_create_ref(o3);
  
  list_t list;
  create_list(&list, malloc, free);
  list_push_back(&list, ro1);
  list_push_back(&list, ro2);
  
  plobj_t *lsobj = plobj_create_list(list);
  
  gc_start();
  gc_mark_white(o4);
  gc_mark_white(lsobj);
  
  assert(o4->gcmark == GCM_WHITE);
  assert(lsobj->gcmark == GCM_WHITE);
  assert(ro1->gcmark == GCM_WHITE);
  assert(ro2->gcmark == GCM_WHITE);
  assert(ro3->gcmark == GCM_BLACK);
  assert(o1->gcmark == GCM_WHITE);
  assert(o2->gcmark == GCM_WHITE);
  assert(o3->gcmark == GCM_BLACK);
  
  gc_cleanup();
  
  assert(o4->used == 1);
  assert(lsobj->used == 1);
  assert(ro1->used == 1);
  assert(ro2->used == 1);
  assert(ro3->used == 0);
  assert(o1->used == 1);
  assert(o2->used == 1);
  assert(o3->used == 0);
  
  gc_start();
  gc_cleanup();
  
  close_heap();
  
  return 0;
}
