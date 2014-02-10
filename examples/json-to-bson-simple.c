#include <stdio.h>

#include <bson.h>
#include <j2bson.h>

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

    n = fread(buf, 1, sizeof(buf) - 1, stdin);
    if (!feof(stdin)) {
        fprintf(stderr, "Input too large\n");
        return 3;
    }

    buf[n] = 0;
    options = 0;

    if (!j2bson_parse_string(buf, n, options, &bson)) {
        fprintf(stderr, "Parse failed\n");
        return 4;
    }

    fwrite(bson_get_data(&bson), 1, bson.len, f);
    bson_destroy(&bson);

    return 0;
}
