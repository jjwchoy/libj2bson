#include "j2bson.h"

#include <stdio.h>
#include <string.h>

#include <yajl/yajl_parse.h>

enum {
    MAX_DEPTH = 32
};

typedef struct j2bson_parser_t {
    yajl_handle yajl_handle;
    j2bson_alloc_funcs_t alloc_funcs;
    j2bson_on_document_func callback;
    void *callback_context;
    bson_t stack[MAX_DEPTH];
    int array_stack[MAX_DEPTH];
    int level;
    const char *key;
    size_t key_length;
    char buf[11];
} j2bson_parser_t;

static int
_j2bson_inc_array(j2bson_parser_t *context);

static int
_j2bson_on_null(void *context);

static int
_j2bson_on_boolean(void *context, int bool_val);

static int
_j2bson_on_integer(void *context, long long integer_val);

static int
_j2bson_on_double(void *context, double double_val);

static int
_j2bson_on_string(void *context, const unsigned char *string_val,
        size_t string_len);

static int
_j2bson_on_start_map(void *context);

static int
_j2bson_on_map_key(void *context, const unsigned char *key,
        size_t key_len);

static int
_j2bson_on_end_map(void *context);

static int
_j2bson_on_start_array(void *context);

static int
_j2bson_on_end_array(void *context);

static const yajl_callbacks _yajl_callbacks = {
     _j2bson_on_null
  ,  _j2bson_on_boolean
  ,  _j2bson_on_integer
  , _j2bson_on_double
  , NULL
  , _j2bson_on_string
  , _j2bson_on_start_map
  , _j2bson_on_map_key
  , _j2bson_on_end_map
  , _j2bson_on_start_array
  , _j2bson_on_end_array
};

static void
_j2bson_on_document(void *context, const bson_t *document);

static void *
_j2bson_malloc(void *context, size_t size);

static void *
_j2bson_realloc(void *context, void *ptr, size_t size);

static void
_j2bson_free(void *context, void *ptr);

static j2bson_alloc_funcs_t _j2bson_default_allocs = {
    _j2bson_malloc
  , _j2bson_realloc
  , _j2bson_free
  , NULL
};

bson_bool_t
j2bson_parse_string(const char *str, size_t str_len, bson_uint32_t options,
        bson_t *parsed) {
    j2bson_parser_t *parser;
    bson_bool_t ok;

    // Unset the J2BSON_OPTIONS_ALLOW_MULTIPLE flag
    options &= ~J2BSON_OPTIONS_ALLOW_MULTIPLE;

    parser = j2bson_parser_alloc(_j2bson_on_document, options, NULL, parsed);

    if (parser) {
        j2bson_parser_read(parser, str, str_len);
        ok = j2bson_parser_finish(parser);
        j2bson_parser_free(parser);
    }

    return ok;
}

void
_j2bson_on_document(void *context, const bson_t *document) {
    memcpy(context, document, sizeof(bson_t));
}

j2bson_parser_t *
j2bson_parser_alloc(bson_uint32_t options, j2bson_alloc_funcs_t *alloc_funcs,
        j2bson_on_document_func callback, void *context) {
    j2bson_parser_t *parser;

    if (NULL == alloc_funcs) {
        alloc_funcs = &_j2bson_default_allocs;
    }

    parser = alloc_funcs->malloc(alloc_funcs->context, sizeof(j2bson_parser_t));

    if (NULL != parser) {
        yajl_alloc_funcs allocs;

        memset(&allocs, 0, sizeof(allocs));
        memset(parser, 0, sizeof(j2bson_parser_t));

        parser->callback = callback;
        parser->callback_context = context;
        parser->level = -1;

        allocs.malloc = alloc_funcs->malloc;
        allocs.realloc = alloc_funcs->realloc;
        allocs.free = alloc_funcs->free;
        allocs.ctx = alloc_funcs->context;

        memcpy(&parser->alloc_funcs, alloc_funcs, sizeof(j2bson_alloc_funcs_t));
        
        parser->yajl_handle = yajl_alloc(&_yajl_callbacks, &allocs, parser);

        if (parser->yajl_handle) {
            if (options & J2BSON_OPTIONS_ALLOW_COMMENTS) {
                yajl_config(parser->yajl_handle, yajl_allow_comments, 1);
            }

            if (options & J2BSON_OPTIONS_ALLOW_MULTIPLE) {
                yajl_config(parser->yajl_handle, yajl_allow_multiple_values);
            }
        } else {
            // yajl allocation failed
            parser->alloc_funcs.free(parser->alloc_funcs.context, parser);
            parser = NULL;
        }
    }

    return parser;
}

bson_bool_t
j2bson_parser_read(j2bson_parser_t *parser, const char *data, size_t len) {
    return yajl_status_ok == yajl_parse(parser->yajl_handle, data, len);
}

bson_bool_t
j2bson_parser_finish(j2bson_parser_t *parser) {
    return yajl_status_ok == yajl_complete_parse(parser->yajl_handle);
}

void
j2bson_parser_free(j2bson_parser_t *parser) {
    yajl_free(parser->yajl_handle);
    parser->alloc_funcs.free(parser->alloc_funcs.context, parser);
}

int
_j2bson_inc_array(j2bson_parser_t *context) {
    if (context->array_stack[context->level] >= 0) {
        snprintf(context->buf, sizeof(context->buf), "%d",
                context->array_stack[context->level]);
        context->key = context->buf;
        context->key_length = strlen(context->key);
        ++context->array_stack[context->level];
    }
}

int
_j2bson_on_null(void *context) {
    j2bson_parser_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_null(ctx->stack + ctx->level, ctx->key,
            ctx->key_length);
}

int
_j2bson_on_boolean(void *context, int bool_val) {
    j2bson_parser_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_bool(ctx->stack + ctx->level, ctx->key,
            ctx->key_length, !!bool_val);
}

int
_j2bson_on_integer(void *context, long long integer_val) {
    j2bson_parser_t *ctx = context;
    _j2bson_inc_array(ctx);
    if (integer_val > INT32_MAX || integer_val < INT32_MIN) {
        return bson_append_int64(ctx->stack + ctx->level, ctx->key,
                ctx->key_length, (bson_int64_t)integer_val);
    } else {
        return bson_append_int32(ctx->stack + ctx->level, ctx->key,
                ctx->key_length, (bson_int32_t)integer_val);
    }
}

int
_j2bson_on_double(void *context, double double_val) {
    j2bson_parser_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_double(ctx->stack + ctx->level, ctx->key,
            ctx->key_length, double_val);
}

int
_j2bson_on_string(void *context, const unsigned char *string_val,
        size_t string_len) {
    j2bson_parser_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_utf8(ctx->stack + ctx->level, ctx->key,
            ctx->key_length, (const char *)string_val, string_len);
}

int
_j2bson_on_start_map(void *context) {
    j2bson_parser_t *ctx = context;
    ++ctx->level;
    if (0 == ctx->level) {
        // start a bson document
        bson_init(ctx->stack);
        ctx->array_stack[0] = -1;
        return 1;
    } else {
        if (ctx->level >= MAX_DEPTH) {
            return 0;
        }

        _j2bson_inc_array(ctx);

        ctx->array_stack[ctx->level] = -1;

        return bson_append_document_begin(ctx->stack + ctx->level - 1,
                ctx->key, ctx->key_length, ctx->stack + ctx->level);
    }
}

int
_j2bson_on_map_key(void *context, const unsigned char *key,
        size_t key_len) {
    j2bson_parser_t *ctx = context;
    ctx->key = key;
    ctx->key_length = key_len;
    return 1;
}

int
_j2bson_on_end_map(void *context) {
    j2bson_parser_t *ctx = context;
    --ctx->level;
    if (-1 == ctx->level) {
        // call the callback func
        ctx->callback(ctx->callback_context, ctx->stack);
        return 1;
    }
    return bson_append_document_end(ctx->stack + ctx->level,
            ctx->stack + ctx->level + 1);
}

int
_j2bson_on_start_array(void *context) {
    j2bson_parser_t *ctx = context;

    ++ctx->level;

    if (0 == ctx->level) {
        // start a bson document
        bson_init(ctx->stack);
        ctx->array_stack[0] = 0;
        return 1;
    } else {
        if (ctx->level >= MAX_DEPTH) {
            return 0;
        }

        _j2bson_inc_array(ctx);

        ctx->array_stack[ctx->level] = 0;
        return bson_append_array_begin(ctx->stack + ctx->level - 1,
                ctx->key, ctx->key_length, ctx->stack + ctx->level);
    }
}

int
_j2bson_on_end_array(void *context) {
    j2bson_parser_t *ctx = context;
    --ctx->level;

    if (-1 == ctx->level) {
        // call the callback
        ctx->callback(ctx->callback_context, ctx->stack);
        return 1;
    }

    return bson_append_array_end(ctx->stack + ctx->level,
            ctx->stack + ctx->level + 1);
}

void *
_j2bson_malloc(void *context, size_t size) {
    return bson_malloc(size);
}

void *
_j2bson_realloc(void *context, void *ptr, size_t size) {
    return bson_realloc(ptr, size);
}

void
_j2bson_free(void *context, void *ptr) {
    bson_free(ptr);
}
