#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "2lex.h"
#include "9global.h"

enum state {
    STATE_IDLE,
    STATE_WORD,
    STATE_NUMBER_INT,
    STATE_NUMBER_FLOAT,
    STATE_STRING1,
    STATE_STRING2,
    STATE_COMMENT,
    STATE_POINT,
};

void lex_dump(struct token_node *tn_start, struct token_node *tn_end, char *file_content) {
    struct token_node *tn = tn_start;
    while (1) {
        switch (tn->token) {
        case TOKEN_START:
            puts("START");
            break;
        case TOKEN_END:
            printf("END");
            break;
        case TOKEN_POINT:
            printf(". ");
            break;
        case TOKEN_EQUALS:
            printf("= ");
            break;
        case TOKEN_PLUS:
            printf("+ ");
            break;
        case TOKEN_MINUS:
            printf("- ");
            break;
        case TOKEN_MULTIPLY:
            printf("* ");
            break;
        case TOKEN_DIVIDE:
            printf("/ ");
            break;
        case TOKEN_SEMICOLON:
            puts(";");
            break;
        case TOKEN_LEFT_PARENTHESIS:
            printf("( ");
            break;
        case TOKEN_RIGHT_PARENTHESIS:
            printf(") ");
            break;
        case TOKEN_WORD: {
            struct token_span *tn2 = (struct token_span *)tn;
            printf("WORD[%s] ", file_content + tn2->index);
            break;
        }
        case TOKEN_NUMBER: {
            struct token_span *tn2 = (struct token_span *)tn;
            printf("NUMBER[%s] ", file_content + tn2->index);
            break;
        }
        case TOKEN_STRING: {
            struct token_span *tn2 = (struct token_span *)tn;
            printf("STRING['%s'] ", file_content + tn2->index);
            break;
        }
        case TOKEN_KEYWORD_LET:
            printf("LET ");
            break;
        case TOKEN_TMPVAR: {
            struct token_tmpvar *tn2 = (struct token_tmpvar *)tn;
            printf("TMPVAR[%d] ", tn2->tmpvar_id);
            break;
        }
        case TOKEN_COMMA:
            printf(", ");
            break;
        default:
            printf("EXIT: UnknownToken:%d\n", tn->token);
            exit(0);
            break;
        }
        if (tn == tn_end)
            break;
        tn = tn->next;
    }
    printf("\n");
}

static void lex_end(struct readed_file *readed) {
    struct token_node *tn = readed->tn_start;
    while (tn) {
        switch (tn->token) {
        case TOKEN_WORD:
        case TOKEN_NUMBER:
        case TOKEN_STRING: {
            struct token_span *tn2 = (struct token_span *)tn;
            readed->file_content[tn2->index + tn2->length] = 0;
            break;
        }
        default:
            break;
        }
        tn = tn->next;
    }
}

void lex(struct readed_file *readed) {

    GHashTable *map_word_to_token = g_hash_table_new(g_str_hash, g_str_equal);
    enum token tn_new = TOKEN_OPERATOR_NEW;
    enum token tn_typeof = TOKEN_OPERATOR_TYPEOF;
    enum token tn_void = TOKEN_OPERATOR_VOID;
    enum token tn_delete = TOKEN_OPERATOR_DELETE;
    enum token tn_in = TOKEN_OPERATOR_IN;
    enum token tn_instanceof = TOKEN_OPERATOR_INSTANCEOF;
    enum token tn_yield = TOKEN_OPERATOR_YIELD;
    enum token tn_let = TOKEN_KEYWORD_LET;
    g_hash_table_insert(map_word_to_token, "new", (void *)&tn_new);
    g_hash_table_insert(map_word_to_token, "typeof", (void *)&tn_typeof);
    g_hash_table_insert(map_word_to_token, "void", (void *)&tn_void);
    g_hash_table_insert(map_word_to_token, "delete", (void *)&tn_delete);
    g_hash_table_insert(map_word_to_token, "in", (void *)&tn_in);
    g_hash_table_insert(map_word_to_token, "instanceof", (void *)&tn_instanceof);
    g_hash_table_insert(map_word_to_token, "yield", (void *)&tn_yield);
    g_hash_table_insert(map_word_to_token, "let", (void *)&tn_let);

    struct token_node *tn = (struct token_node *)malloc(sizeof(struct token_node));
    tn->token = TOKEN_START;
    tn->next = NULL;
    tn->prev = NULL;

    struct token_node *tn_start = tn;

    enum state state = STATE_IDLE;
    long state_helper;
    for (long i = 0; i < readed->file_size; i++) {
        char c = readed->file_content[i];
        if (state == STATE_IDLE) {
            if (c == ' ') {
                ;
            } else if (c == '.') {
                state = STATE_POINT;
                state_helper = i;
            } else if (c == '=') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_EQUALS;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '+') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_PLUS;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '-') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_MINUS;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '*') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_MULTIPLY;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '/') {
                if( tn->token == TOKEN_DIVIDE ){
                    tn = tn->prev;
                    free(tn->next);
                    tn->next = NULL;
                    state = STATE_COMMENT;
                    continue;
                } else {
                    struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                    tn2->token = TOKEN_DIVIDE;
                    tn2->next = NULL;
                    tn2->prev = tn;
                    tn->next = tn2;
                    tn = tn2;
                }
            } else if (c == ';') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_SEMICOLON;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '(') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_LEFT_PARENTHESIS;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == ')') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_RIGHT_PARENTHESIS;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '[') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_LEFT_SQUAREBRACKET;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == ']') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_RIGHT_SQUAREBRACKET;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == ',') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_COMMA;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '~') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_TILDE;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == ':') {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_COLON;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;
            } else if (c == '\n') {
                ;
            } else if (c == '"') {
                state = STATE_STRING1;
                state_helper = i + 1;
            } else if (c == '\'') {
                state = STATE_STRING2;
                state_helper = i + 1;
            } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                state = STATE_WORD;
                state_helper = i;
            } else if (c >= '0' && c <= '9') {
                state = STATE_NUMBER_INT;
                state_helper = i;
            } else {
                printf("EXIT: UnknownChar:%c\n", c);
                exit(0);
            }
        } else if (state == STATE_WORD) {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {

                readed->file_content[i] = '\0';
                enum token *ret = (enum token *)g_hash_table_lookup(map_word_to_token, readed->file_content + state_helper);
                readed->file_content[i] = c;

                if (ret) {
                    struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                    tn2->token = *ret;
                    tn2->next = NULL;
                    tn2->prev = tn;
                    tn->next = tn2;
                    tn = tn2;
                } else {
                    struct token_span *tn2 = (struct token_span *)malloc(sizeof(struct token_span));
                    tn2->token = TOKEN_WORD;
                    tn2->next = NULL;
                    tn2->prev = tn;
                    tn2->index = state_helper;
                    tn2->length = i - state_helper;
                    tn->next = (struct token_node *)tn2;
                    tn = (struct token_node *)tn2;
                }

                state = STATE_IDLE;
                i--;
            }
        } else if (state == STATE_NUMBER_INT) {
            if (c == '.') {
                state = STATE_NUMBER_FLOAT;
            } else if (!(c >= '0' && c <= '9')) {
                struct token_span *tn2 = (struct token_span *)malloc(sizeof(struct token_span));
                tn2->token = TOKEN_NUMBER;
                tn2->next = NULL;
                tn2->prev = tn;
                tn2->index = state_helper;
                tn2->length = i - state_helper;
                tn->next = (struct token_node *)tn2;
                tn = (struct token_node *)tn2;

                state = STATE_IDLE;
                i--;
            }
        } else if (state == STATE_NUMBER_FLOAT) {
            if (!(c >= '0' && c <= '9')) {
                struct token_span *tn2 = (struct token_span *)malloc(sizeof(struct token_span));
                tn2->token = TOKEN_NUMBER;
                tn2->next = NULL;
                tn2->prev = tn;
                tn2->index = state_helper;
                tn2->length = i - state_helper;
                tn->next = (struct token_node *)tn2;
                tn = (struct token_node *)tn2;

                state = STATE_IDLE;
                i--;
            }
        } else if (state == STATE_STRING1) {
            if (c == '"') {
                struct token_span *tn2 = (struct token_span *)malloc(sizeof(struct token_span));
                tn2->token = TOKEN_STRING;
                tn2->next = NULL;
                tn2->prev = tn;
                tn2->index = state_helper;
                tn2->length = i - state_helper;
                tn->next = (struct token_node *)tn2;
                tn = (struct token_node *)tn2;

                state = STATE_IDLE;
            }
        } else if (state == STATE_STRING2) {
            if (c == '\'') {
                struct token_span *tn2 = (struct token_span *)malloc(sizeof(struct token_span));
                tn2->token = TOKEN_STRING;
                tn2->next = NULL;
                tn2->prev = tn;
                tn2->index = state_helper;
                tn2->length = i - state_helper;
                tn->next = (struct token_node *)tn2;
                tn = (struct token_node *)tn2;

                state = STATE_IDLE;
            }
        } else if (state == STATE_POINT) {
            if (c >= '0' && c <= '9') {
                state = STATE_NUMBER_FLOAT;
            } else {
                struct token_node *tn2 = (struct token_node *)malloc(sizeof(struct token_node));
                tn2->token = TOKEN_POINT;
                tn2->next = NULL;
                tn2->prev = tn;
                tn->next = tn2;
                tn = tn2;

                state = STATE_IDLE;
                i--;
            }
        } else if (state == STATE_COMMENT) {
            if (c == '\n') {
                state = STATE_IDLE;
            }
        } else {
            printf("EXIT: UnknownState=%d\n", state);
            exit(0);
        }
    }

    struct token_node *tn_end = (struct token_node *)malloc(sizeof(struct token_node));
    tn_end->token = TOKEN_END;
    tn_end->next = NULL;
    tn_end->prev = tn;
    tn->next = tn_end;

    readed->tn_start = tn_start;
    readed->tn_end = tn_end;

    lex_end(readed);

#ifdef DEBUG
    puts("---lex_dump---");
    lex_dump(readed->tn_start, readed->tn_end, readed->file_content);
    puts("---lex_dump---");
#endif
}
