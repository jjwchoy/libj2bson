#include <stdio.h>

#include <bson.h>
#include <j2bson.h>

static void
_j2b_on_doc(void *context, const bson_t *document);

int
main(int argc, char *argv[]) {
    bson_t bson;
    const char *filename;
    j2bson_handle_t parser;
    bson_uint32_t options;
    char buf[BUFSIZ];
    size_t n;

    if (argc == 1) {
        fprintf(stderr, "usage: %s OUTFILE\n", argv[0]);
        return 1;
    }

    FILE *f;
    filename = argv[1];
    f = fopen(filename, "wb");
    if (NULL == f) {
        return 2;
    }

    options = J2BSON_OPTIONS_ALLOW_MULTIPLE;
    parser = j2bson_parser_alloc(options, NULL, _j2b_on_doc, f);

    if (!parser) {
        fprintf(stderr, "Parser allocation failed\n");
        return 3;
    }

    for (;;) {
        n = fread(buf, 1, sizeof(buf) - 1, stdin);
        if (n == 0) {
            if (!feof(stdin)) {
                fprintf(stderr, "Read failed\n");
                return 4;
            }
            break;
        }

        buf[n] = 0;

        if (!j2bson_parser_read(parser, buf, n)) {
            break;
        }
    }

    if (!j2bson_parser_finish(parser)) {
        fprintf(stderr, "Parse failed\n");
        return 5;

    }

    fclose(f);
    j2bson_parser_free(parser);

    return 0;
}

void
_j2b_on_doc(void *context, const bson_t *document) {
    fwrite(bson_get_data(document), 1, document->len, (FILE *)context);
}
