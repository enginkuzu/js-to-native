#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "1read.h"
#include "9global.h"

static long get_file_size(char *filename) {
    struct stat file_status;
    if (stat(filename, &file_status) < 0) {
        return -1;
    }
    // printf("file size : %ld\n", file_status.st_size);
    return file_status.st_size;
}

static char *get_file_content(char *file_name, long file_size) {
    char *buffer = (char *)malloc(file_size + 1);
    FILE *fp = fopen(file_name, "r");
    fread(buffer, file_size + 1, 1, fp);
    fclose(fp);
    // printf("file content : [%s]\n", buffer);
    return buffer;
}

struct readed_file *read(char *file_name) {
    long file_size = get_file_size(file_name);
    if (file_size < 0) {
        printf("EXIT: File %s not found!\n", file_name);
        exit(0);
    }
    char *file_content = get_file_content(file_name, file_size);
    struct readed_file *ret = (struct readed_file *)malloc(sizeof(struct readed_file));
    ret->file_size = file_size;
    ret->file_content = file_content;
    ret->tn_start = NULL;
    ret->tn_end = NULL;
#ifdef DEBUG
    puts("---read_dump---");
    printf("%ld byte\n%s\n", ret->file_size, ret->file_content);
    puts("---read_dump---");
#endif
    return ret;
};
