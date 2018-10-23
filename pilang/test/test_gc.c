#include "plheap.h"

#include "mstring.h"
#include "vktest.h"

int main() {
  VK_TEST_BEGIN

  init_heap();
  
  plobj_t *o1 = plobj_create_int(39);
  plobj_t *o2 = plobj_create_float(39.0);
  plobj_t *o3 = plobj_create_str(create_string("fuck"));
  plobj_t *o4 = plobj_create_str(create_string("shit"));
  
  list_t list;
  create_list(&list, malloc, free);
  list_push_back(&list, o1);
  list_push_back(&list, o2);
  
  plobj_t *lsobj = plobj_create_list(list);
  
  gc_start();
  gc_mark_white(o4);
  gc_mark_white(lsobj);
  
  VK_ASSERT_EQUALS(GCM_WHITE, o4->gcmark);
  VK_ASSERT_EQUALS(GCM_WHITE, lsobj->gcmark);
  VK_ASSERT_EQUALS(GCM_WHITE, o1->gcmark);
  VK_ASSERT_EQUALS(GCM_WHITE, o2->gcmark);
  VK_ASSERT_EQUALS(GCM_BLACK, o3->gcmark);
  
  gc_cleanup();
  
  VK_ASSERT_EQUALS(1, o4->used);
  VK_ASSERT_EQUALS(1, lsobj->used);
  VK_ASSERT_EQUALS(1, o1->used);
  VK_ASSERT_EQUALS(1, o2->used);
  VK_ASSERT_EQUALS(0, o3->used);
  
  gc_start();
  gc_cleanup();
  close_heap();

  VK_TEST_END

  return 0;
}
