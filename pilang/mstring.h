#ifndef CMALT_MSTRING_H
#define CMALT_MSTRING_H

#include <stdint.h>

#define STRING_HASHMAP_BUCKET_COUNT 4396

extern int64_t create_string(const char* str);
extern char const* get_string(int64_t hStr);

#endif // CMALT_MSTRING_H
