#pragma once

#include <stdint.h>

#include "1read.h"

enum token {
    TOKEN_START,
    TOKEN_END,
    TOKEN_TMPVAR,
    TOKEN_WORD,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_POINT,
    TOKEN_EQUALS,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_SEMICOLON,
    TOKEN_LEFT_PARENTHESIS,
    TOKEN_RIGHT_PARENTHESIS,
    TOKEN_LEFT_SQUAREBRACKET,
    TOKEN_RIGHT_SQUAREBRACKET,
    TOKEN_COMMA,
    TOKEN_TILDE,
    TOKEN_COLON,
    TOKEN_OPERATOR_NEW,
    TOKEN_OPERATOR_TYPEOF,
    TOKEN_OPERATOR_VOID,
    TOKEN_OPERATOR_DELETE,
    TOKEN_OPERATOR_IN,
    TOKEN_OPERATOR_INSTANCEOF,
    TOKEN_OPERATOR_YIELD,
    TOKEN_KEYWORD_LET,
};

struct token_node {
    enum token token;
    struct token_node *next;
    struct token_node *prev;
};

// TOKEN_WORD
// TOKEN_NUMBER
// TOKEN_STRING
struct token_span {
    enum token token;
    struct token_node *next;
    struct token_node *prev;
    long index;
    long length;
};

// TOKEN_TMPVAR
struct token_tmpvar {
    enum token token;
    struct token_node *next;
    struct token_node *prev;
    uint32_t tmpvar_id;
};

void lex(struct readed_file *readed);
void lex_dump(struct token_node *tn_start, struct token_node *tn_end, char *file_content);
