#ifndef CMALT_MSTRING_H
#define CMALT_MSTRING_H

#include <stdint.h>

#define STRING_HASHMAP_BUCKET_COUNT 4396

#ifdef __cplusplus
extern "C" {
#endif

int64_t create_string(const char* str);
char const* get_string(int64_t hstr);

#ifdef __cplusplus
}
#endif

#endif // CMALT_MSTRING_H
