#include <stdio.h>
#include <stdlib.h>

#include "1read.h"
#include "2lex.h"
#include "3parse.h"
#include "4optimize.h"
#include "5save.h"
#include "9global.h"

static char *src_file_name;
static char *dst_file_name;

void cli(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage : ./main test/test.js -o test-build/test.c\n");
        exit(0);
    }
    src_file_name = argv[1];
    dst_file_name = argv[3];
};

int main(int argc, char **argv) {

#ifdef DEBUG
    puts("---main---");
#endif

    cli(argc, argv);
    struct readed_file *readed = read(src_file_name);
    lex(readed);
    struct parser_result *parsed = parse(readed);
    optimize();
    save(dst_file_name, parsed, readed->file_content);

    // Free operations
    free(parsed->tmpvars);
    free(readed->file_content);
    free(readed);

#ifdef DEBUG
    puts("---main---");
#endif

    return 0;
}
