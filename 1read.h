#pragma once

struct readed_file {
    long file_size;
    char *file_content;
    struct token_node *tn_start;
    struct token_node *tn_end;
};

struct readed_file *read(char *file_name);
