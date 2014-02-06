#include <stdio.h>

#include <bson.h>
#include <j2bson.h>

int
main(int argc, char *argv[]) {
    bson_t bson;
    const char *filename;

    if (argc == 1) {
        fprintf(stderr, "usage: %s OUTFILE\n", argv[0]);
        return 1;
    }

    if (j2bson_init(&bson, stdin, NULL)) {
        FILE *f;
        filename = argv[1];
        f = fopen(filename, "wb");
        if (NULL == f) {
            return 4;
        }
        fwrite(bson_get_data(&bson), 1, bson.len, f);

        if (fclose(f)) {
            return 5;
        }
    } else {
        return 3;
    }

    return 0;
}
