#include "j2bson.h"

#include <stdio.h>
#include <string.h>

#include <yajl/yajl_parse.h>

enum {
    MAX_DEPTH = 32
};

typedef struct j2bson_context_t {
    bson_t stack[MAX_DEPTH];
    int array_stack[MAX_DEPTH];
    int level;
    const char *key;
    size_t key_length;
    char buf[11];
} j2bson_context_t;

static int
_j2bson_inc_array(j2bson_context_t *context);

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

bson_bool_t
j2bson_init(bson_t *bson, FILE *stream, size_t *bytes_consumed) {
    yajl_handle yajl_handle;
    j2bson_context_t context;
    char buf[8192];
    bson_bool_t ok = TRUE;
    size_t initial_pos = ftell(stream);
    size_t bytes_parsed;
    size_t n;

    // Initialise context
    memset(&context, 0, sizeof(context));
    bson_init(context.stack);
    context.array_stack[0] = -1;

    yajl_handle = yajl_alloc(&_yajl_callbacks, NULL, &context);
    yajl_config(yajl_handle, yajl_allow_trailing_garbage, 1);

    for (;;) {
        n = fread(buf, 1, sizeof(buf) - 1, stream);
        if (n == 0) { 
            if (!feof(stdin)) {
                ok = FALSE;
            }
            break;
        }

        buf[n] = 0;
            
        if (yajl_status_ok != yajl_parse(yajl_handle, buf, n)) {
            ok = FALSE;
            break;
        }
    }

    ok = yajl_status_ok == yajl_complete_parse(yajl_handle);

    bytes_parsed = ok ? yajl_get_bytes_consumed(yajl_handle) : 0;
    fseek(stream, initial_pos + bytes_parsed, SEEK_SET);

    if (bytes_consumed) {
        *bytes_consumed = bytes_parsed;
    }

    yajl_free(yajl_handle);

    if (ok) {
        memcpy(bson, context.stack, sizeof(bson_t));
    } else {
        bson_destroy(context.stack);
    }

    return ok;
}

bson_bool_t
j2bson_init_from_string(bson_t *bson, const char *str, size_t len,
        size_t *bytes_consumed) {
    FILE *stream = fmemopen((char *)str, len, "rb");
    bson_bool_t ok = j2bson_init(bson, stream, bytes_consumed);
    fclose(stream);
    return ok;
}

int
_j2bson_inc_array(j2bson_context_t *context) {
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
    j2bson_context_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_null(ctx->stack + ctx->level, ctx->key,
            ctx->key_length);
}

int
_j2bson_on_boolean(void *context, int bool_val) {
    j2bson_context_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_bool(ctx->stack + ctx->level, ctx->key,
            ctx->key_length, !!bool_val);
}

int
_j2bson_on_integer(void *context, long long integer_val) {
    j2bson_context_t *ctx = context;
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
    j2bson_context_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_double(ctx->stack + ctx->level, ctx->key,
            ctx->key_length, double_val);
}

int
_j2bson_on_string(void *context, const unsigned char *string_val,
        size_t string_len) {
    j2bson_context_t *ctx = context;
    _j2bson_inc_array(ctx);
    return bson_append_utf8(ctx->stack + ctx->level, ctx->key,
            ctx->key_length, (const char *)string_val, string_len);
}

int
_j2bson_on_start_map(void *context) {
    j2bson_context_t *ctx = context;
    if (0 == ctx->level) {
        return 1;
    }

    _j2bson_inc_array(ctx);

    ++ctx->level;

    if (ctx->level >= MAX_DEPTH) {
        return 0;
    }

    ctx->array_stack[ctx->level] = -1;

    return bson_append_document_begin(ctx->stack + ctx->level - 1,
            ctx->key, ctx->key_length, ctx->stack + ctx->level);
}

int
_j2bson_on_map_key(void *context, const unsigned char *key,
        size_t key_len) {
    j2bson_context_t *ctx = context;
    ctx->key = key;
    ctx->key_length = key_len;
    return 1;
}

int
_j2bson_on_end_map(void *context) {
    j2bson_context_t *ctx = context;
    if (0 == ctx->level) {
        return 1;
    }
    --ctx->level;
    return bson_append_document_end(ctx->stack + ctx->level,
            ctx->stack + ctx->level + 1);
}

int
_j2bson_on_start_array(void *context) {
    j2bson_context_t *ctx = context;
    _j2bson_inc_array(ctx);

    ++ctx->level;

    if (ctx->level >= MAX_DEPTH) {
        return 0;
    }

    ctx->array_stack[ctx->level] = 0;
    return bson_append_array_begin(ctx->stack + ctx->level - 1,
            ctx->key, ctx->key_length, ctx->stack + ctx->level);
}

int
_j2bson_on_end_array(void *context) {
    j2bson_context_t *ctx = context;
    --ctx->level;
    return bson_append_array_end(ctx->stack + ctx->level,
            ctx->stack + ctx->level + 1);
}
