/*
 * Copyright 2014 Jason Choy <jjwchoy@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef J2BSON_H
#define J2BSON_H

#include <stddef.h>
#include <bson.h>

BSON_BEGIN_DECLS

typedef enum {
    J2BSON_OPTIONS_ALLOW_COMMENTS = 1 << 0
  , J2BSON_OPTIONS_ALLOW_MULTIPLE = 1 << 1
} j2bson_options_t;

typedef void (*j2bson_on_document_func)(void *context, bson_t *document);

struct yajl_handle_t;
struct j2bson_context_t;

typedef struct j2bson_parser_t j2bson_parser_t;
typedef j2bson_parser_t * j2bson_handle_t;

typedef void *(*j2bson_malloc_func)(void *ctx, size_t size);
typedef void (*j2bson_free_func)(void *ctx, void *ptr);
typedef void *(*j2bson_realloc_func)(void *ctx, void *ptr, size_t size);

typedef struct j2bson_alloc_funcs_t {
    j2bson_malloc_func malloc;
    j2bson_realloc_func realloc;
    j2bson_free_func free;
    void *context;
} j2bson_alloc_funcs_t;

// Simple API
bson_bool_t
j2bson_parse_string(const char *str, size_t str_len, bson_uint32_t options,
        bson_t *parsed);

// Streaming API
j2bson_handle_t
j2bson_parser_alloc(bson_uint32_t options, j2bson_alloc_funcs_t *alloc_funcs,
        j2bson_on_document_func callback, void *context);

bson_bool_t
j2bson_parser_read(j2bson_handle_t parser, const char *data, size_t len);

bson_bool_t
j2bson_parser_finish(j2bson_handle_t parser);

void
j2bson_parser_free(j2bson_handle_t parser);

BSON_END_DECLS

#endif
