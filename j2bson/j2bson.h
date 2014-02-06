#ifndef J2BSON_H
#define J2BSON_H

#include <stddef.h>
#include <bson.h>

BSON_BEGIN_DECLS

bson_bool_t
j2bson_init(bson_t *b, FILE *stream, size_t *bytes_consumed);

bson_bool_t
j2bson_init_from_string(bson_t *b, const char *str, size_t len,
        size_t *bytes_consumed);

BSON_END_DECLS

#endif
