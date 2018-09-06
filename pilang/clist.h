#ifndef CLIST_H
#define CLIST_H

#include <stddef.h>

struct _clistimpl;

typedef struct _clist {
    struct _clistimpl *impl;
} list_t;

typedef struct _citerator {
    void* opaque;
} iter_t;

typedef struct _cciterator {
    const void* opaque;
} const_iter_t;

typedef void*(*allocator_t)(size_t);
typedef void(*deallocator_t)(void*);
typedef void *any_t;
typedef const void *const_any_t;

void create_list(list_t *list, allocator_t alloc, deallocator_t dealloc);
void destroy_list(list_t *list);
void list_push_back(list_t *list, any_t any);
void list_push_front(list_t *list, any_t any);
void list_insert_before(list_t *list, iter_t iter, any_t any);
void list_insert_after(list_t *list, iter_t iter, any_t ant);
void list_remove(list_t *list, iter_t iter);
iter_t list_begin(list_t *list);
iter_t list_end(list_t *list);
const_iter_t list_cbegin(list_t const* list);
const_iter_t list_cend(list_t const* list);

any_t iter_deref(iter_t iter);
const_any_t const_iter_deref(const_iter_t iter);
iter_t iter_next(iter_t iter);
iter_t iter_prev(iter_t iter);
const_iter_t const_iter_next(const_iter_t iter);
const_iter_t const_iter_prev(const_iter_t iter);


#endif // CLIST_H