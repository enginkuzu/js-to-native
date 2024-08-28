#include <glib-2.0/glib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "3parse.h"
#include "9global.h"
#include "js_function.h"

static uint32_t last_tmpvar_id = 0;

static char *merge(char *str1, char *str2, char *str3) {
    int bindex = 0;
    char *ret = (char *)malloc(strlen(str1) + strlen(str2) + strlen(str3) + 1);
    bindex += sprintf(ret + bindex, "%s", str1);
    bindex += sprintf(ret + bindex, "%s", str2);
    bindex += sprintf(ret + bindex, "%s", str3);
    ret[bindex] = 0;
    return ret;
}

static char *get_jstype_string(enum jsvar_type js_type) {
    switch (js_type) {
    case JS_STRING:
        return "String";
        break;
    default:
        printf("EXIT: Unknown JS Type : %d\n", js_type);
        exit(0);
        return NULL;
        break;
    }
}

#ifdef DEBUG
static char *get_string(struct parser_result *result, uint32_t tmpvar_id, char *file_content) {
    long index = result->tmpvars[tmpvar_id].index;
    if (index == 0)
        return "";
    return file_content + index;
}

static void parser_dump(struct parser_result *result, char *file_content) {
    struct parser_task0 *task = result->task_start;
    while (task != NULL) {
        switch (task->fn) {
        case START:
            printf("START\n");
            break;
        case END:
            printf("END\n");
            break;
        case ASSIGN: {
            struct parser_task1 *task1 = (struct parser_task1 *)task;
            printf("ASSIGN src1=%d=[%s]:%d dst=%d:%d\n", task1->src, get_string(result, task1->src, file_content), result->tmpvars[task1->src].js_type, task1->dst, result->tmpvars[task1->dst].js_type);
            break;
        }
        case FUNCTION_CALL: {
            struct parser_task2 *task2 = (struct parser_task2 *)task;
            printf("FUNCTION_CALL fn_name=[%s] src1=%d=[%s]:%d src2=%d src3=%d dst=%d:%d\n", task2->fn_name, task2->src1, get_string(result, task2->src1, file_content), result->tmpvars[task2->src1].js_type, task2->src2, task2->src3, task2->dst, result->tmpvars[task2->dst].js_type);
            break;
        }
        case DOUBLE_ADD:
        case DOUBLE_SUB:
        case DOUBLE_MUL:
        case DOUBLE_DIV:
        case STRING_ADD: {
            struct parser_task3 *task3 = (struct parser_task3 *)task;
            if (task->fn == DOUBLE_ADD) {
                printf("DOUBLE_ADD");
            } else if (task->fn == DOUBLE_SUB) {
                printf("DOUBLE_SUB");
            } else if (task->fn == DOUBLE_MUL) {
                printf("DOUBLE_MUL");
            } else if (task->fn == DOUBLE_DIV) {
                printf("DOUBLE_DIV");
            } else if (task->fn == STRING_ADD) {
                printf("STRING_ADD");
            } else {
                printf("EXIT: TODO8\n");
                exit(0);
            }
            printf(" src1=%d=[%s]:%d src2=%d=[%s]:%d dst=%d:%d\n", task3->src1, get_string(result, task3->src1, file_content), result->tmpvars[task3->src1].js_type, task3->src2, get_string(result, task3->src2, file_content), result->tmpvars[task3->src2].js_type, task3->dst, result->tmpvars[task3->dst].js_type);
            break;
        }
        default:
            printf("EXIT: UnknownFn:%d\n", task->fn);
            exit(0);
            break;
        }
        task = task->next;
    }
}
#endif

static uint32_t register_tmpvar(struct parser_result *result, enum tmpvar_type type, long index, enum jsvar_type js_type) {
    uint32_t tmpvar_id = ++last_tmpvar_id;
    result->tmpvars[tmpvar_id].type = type;
    result->tmpvars[tmpvar_id].index = index;
    result->tmpvars[tmpvar_id].tmpvar_id = tmpvar_id;
    result->tmpvars[tmpvar_id].js_type = js_type;
    return tmpvar_id;
}

static GHashTable *hash_map;
static GHashTable *map_static_fn_types;

static uint32_t get_uservar(char *file_content, long index) {
    uint32_t *ret = (uint32_t *)g_hash_table_lookup(hash_map, file_content + index);
    if (ret)
        return *ret;
    return 0;
}

static enum jsvar_type get_fn_type(char *fn_name) {
    enum jsvar_type *ret = (enum jsvar_type *)g_hash_table_lookup(map_static_fn_types, fn_name);
    if (ret)
        return *ret;
    return JS_TYPE_NOT_FOUND;
}

static uint32_t register_uservar(struct parser_result *result, enum tmpvar_type type, char *file_content, long index, enum jsvar_type js_type) {
    uint32_t tmpvar_id = register_tmpvar(result, type, index, js_type);
    g_hash_table_insert(hash_map, file_content + index, &result->tmpvars[tmpvar_id].tmpvar_id);
    return tmpvar_id;
}

static void register_operation1(struct parser_result *result, enum parser_fn fn, uint32_t src, uint32_t dst) {

    struct parser_task1 *new = (struct parser_task1 *)malloc(sizeof(struct parser_task1));
    new->fn = fn;
    new->next = NULL;
    new->src = src;
    new->dst = dst;

    result->task_end->next = (struct parser_task0 *)new;
    result->task_end = (struct parser_task0 *)new;
}

static void register_operation2(struct parser_result *result, enum parser_fn fn, char *fn_name, uint32_t src1, uint32_t src2, uint32_t src3, uint32_t dst) {

    struct parser_task2 *new = (struct parser_task2 *)malloc(sizeof(struct parser_task2));
    new->fn = fn;
    new->next = NULL;
    new->fn_name = fn_name;
    new->src1 = src1;
    new->src2 = src2;
    new->src3 = src3;
    new->dst = dst;

    result->task_end->next = (struct parser_task0 *)new;
    result->task_end = (struct parser_task0 *)new;
}

static void register_operation3(struct parser_result *result, enum parser_fn fn, uint32_t src1, uint32_t src2, uint32_t dst) {

    struct parser_task3 *new = (struct parser_task3 *)malloc(sizeof(struct parser_task3));
    new->fn = fn;
    new->next = NULL;
    new->src1 = src1;
    new->src2 = src2;
    new->dst = dst;

    result->task_end->next = (struct parser_task0 *)new;
    result->task_end = (struct parser_task0 *)new;
}

static uint32_t token_to_varid(struct parser_result *result, struct token_node *tn, char *file_content) {
    uint32_t varid;
    if (tn->token == TOKEN_NUMBER) {
        enum jsvar_type jstype = JS_NUMBER;
        varid = register_tmpvar(result, NEW_CONST, ((struct token_span *)tn)->index, jstype);
        uint32_t tmp_var_id = register_tmpvar(result, NEW_TMPVAR, 0, jstype);
        register_operation1(result, ASSIGN, varid, tmp_var_id);
        varid = tmp_var_id;
    } else if (tn->token == TOKEN_WORD) {
        varid = get_uservar(file_content, ((struct token_span *)tn)->index);
        if (varid == 0) {
            printf("EXIT: UserVar [%s] not found\n", file_content + ((struct token_span *)tn)->index);
            exit(0);
        }
    } else if (tn->token == TOKEN_STRING) {
        enum jsvar_type jstype = JS_STRING;
        varid = register_tmpvar(result, NEW_CONST, ((struct token_span *)tn)->index, jstype);
        uint32_t tmp_var_id = register_tmpvar(result, NEW_TMPVAR, 0, jstype);
        register_operation1(result, ASSIGN, varid, tmp_var_id);
        varid = tmp_var_id;
    } else if (tn->token == TOKEN_TMPVAR) {
        varid = ((struct token_tmpvar *)tn)->tmpvar_id;
    } else {
        varid = 0;
        printf("EXIT: Token type [%d] unexpected\n", tn->token);
        exit(0);
    }
    return varid;
}

static void parse2paranthesis(struct parser_result *result, struct token_node *tn_start, struct token_node *tn_end, char *file_content) {

    uint8_t has_muldiv = 0;
    uint8_t has_addsub = 0;
    struct token_node *tn = tn_start;
    while (1) {
        tn = tn->next;
        if (tn == tn_end)
            break;
        if (tn->token == TOKEN_MULTIPLY || tn->token == TOKEN_DIVIDE) {
            has_muldiv = 1;
        } else if (tn->token == TOKEN_PLUS || tn->token == TOKEN_MINUS) {
            has_addsub = 1;
        }
    }

#ifdef DEBUG
    printf("---parse2paranthesis-1---\n");
    lex_dump(tn_start, tn_end, file_content);
    printf("\n");
#endif

    if (has_muldiv) {
        tn = tn_start;
        while (1) {
            tn = tn->next;
            if (tn == tn_end)
                break;
            if (tn->token == TOKEN_MULTIPLY || tn->token == TOKEN_DIVIDE) {

                enum parser_fn fn;
                if (tn->token == TOKEN_MULTIPLY) {
                    fn = DOUBLE_MUL;
                } else if (tn->token == TOKEN_DIVIDE) {
                    fn = DOUBLE_DIV;
                } else {
                    continue;
                }

                uint32_t src1 = token_to_varid(result, tn->prev, file_content);
                uint32_t src2 = token_to_varid(result, tn->next, file_content);

                enum jsvar_type target_jstype;
                if (result->tmpvars[src1].js_type == JS_NUMBER && result->tmpvars[src2].js_type == JS_NUMBER) {
                    target_jstype = JS_NUMBER;
                } else {
                    printf("EXIT: TODO4\n");
                    exit(0);
                }

                uint32_t dst = register_tmpvar(result, NEW_TMPVAR, 0, target_jstype);

                struct token_tmpvar *token = (struct token_tmpvar *)malloc(sizeof(struct token_tmpvar));
                token->token = TOKEN_TMPVAR;
                token->next = tn->next->next;
                token->prev = tn->prev->prev;
                token->next->prev = (struct token_node *)token;
                token->prev->next = (struct token_node *)token;
                token->tmpvar_id = dst;

                // Free operations
                free(tn->prev);
                free(tn->next);
                free(tn);

                register_operation3(result, fn, src1, src2, dst);

                tn = (struct token_node *)token;
            }
        }
    }

    if (has_addsub) {
        tn = tn_start;
        while (1) {
            tn = tn->next;
            if (tn == tn_end)
                break;
            if (tn->token == TOKEN_PLUS || tn->token == TOKEN_MINUS) {

                enum parser_fn fn;
                if (tn->token == TOKEN_PLUS) {
                    fn = DOUBLE_ADD;
                } else if (tn->token == TOKEN_MINUS) {
                    fn = DOUBLE_SUB;
                } else {
                    continue;
                }

                uint32_t src1 = token_to_varid(result, tn->prev, file_content);
                uint32_t src2 = token_to_varid(result, tn->next, file_content);

                enum jsvar_type target_jstype;
                if (tn->token == TOKEN_PLUS) {
                    if (result->tmpvars[src1].js_type == JS_NUMBER && result->tmpvars[src2].js_type == JS_NUMBER) {
                        target_jstype = JS_NUMBER;
                    } else if (result->tmpvars[src1].js_type == JS_STRING || result->tmpvars[src2].js_type == JS_STRING) {
                        target_jstype = JS_STRING;
                        fn = STRING_ADD;
                    } else {
                        printf("EXIT: TODO5\n");
                        exit(0);
                    }
                } else if (tn->token == TOKEN_MINUS) {
                    if (result->tmpvars[src1].js_type == JS_NUMBER && result->tmpvars[src2].js_type == JS_NUMBER) {
                        target_jstype = JS_NUMBER;
                    } else {
                        printf("EXIT: TODO6\n");
                        exit(0);
                    }
                } else {
                    printf("EXIT: TODO7\n");
                    exit(0);
                }

                uint32_t dst = register_tmpvar(result, NEW_TMPVAR, 0, target_jstype);

                struct token_tmpvar *token = (struct token_tmpvar *)malloc(sizeof(struct token_tmpvar));
                token->token = TOKEN_TMPVAR;
                token->next = tn->next->next;
                token->prev = tn->prev->prev;
                token->next->prev = (struct token_node *)token;
                token->prev->next = (struct token_node *)token;
                token->tmpvar_id = dst;

                // Free operations
                free(tn->prev);
                free(tn->next);
                free(tn);

                register_operation3(result, fn, src1, src2, dst);

                tn = (struct token_node *)token;
            }
        }
    }
}

static void do1fncall(struct parser_result *result, struct token_node *tn1, struct token_node *tn2, char *file_content) {

    if (tn1->prev->prev->token == TOKEN_POINT && tn1->prev->prev->prev->token == TOKEN_WORD) {

        long index1 = ((struct token_span *)tn1->prev->prev->prev)->index;
        char *fn_name1 = file_content + index1;
        long index2 = ((struct token_span *)tn1->prev)->index;
        char *fn_name2 = file_content + index2;

        char *fn_name = NULL;
        enum jsvar_type dst_type = JS_TYPE_NOT_FOUND;
        uint32_t dst;
        {
            // Test object fn call
            uint32_t varid = get_uservar(file_content, index1);
            if (varid > 0) {
                enum jsvar_type js_type = result->tmpvars[varid].js_type;
                char *jstype_string = get_jstype_string(js_type);
                fn_name = merge(jstype_string, ".", fn_name2);
                dst_type = get_fn_type(fn_name);
                if (dst_type == JS_TYPE_NOT_FOUND) {
                    free(fn_name);
                } else {
                    dst = register_tmpvar(result, NEW_TMPVAR, 0, dst_type);

                    uint32_t src1 = varid;
                    uint32_t src2 = token_to_varid(result, tn1->next, file_content);
                    uint32_t src3 = tn1->next->next->token == TOKEN_COMMA ? token_to_varid(result, tn1->next->next->next, file_content) : 0;
                    register_operation2(result, FUNCTION_CALL, fn_name, src1, src2, src3, dst);
                }
            }
        }
        if (dst_type == JS_TYPE_NOT_FOUND) {
            // Test static fn call
            fn_name = merge(fn_name1, ".", fn_name2);
            dst_type = get_fn_type(fn_name);
            if (dst_type == JS_TYPE_NOT_FOUND) {
                free(fn_name);
            } else {
                dst = register_tmpvar(result, NEW_TMPVAR, 0, dst_type);

                uint32_t src1 = token_to_varid(result, tn1->next, file_content);
                uint32_t src2 = tn1->next->next->token == TOKEN_COMMA ? token_to_varid(result, tn1->next->next->next, file_content) : 0;
                uint32_t src3 = 0;
                register_operation2(result, FUNCTION_CALL, fn_name, src1, src2, src3, dst);
            }
        }
        if (dst_type == JS_TYPE_NOT_FOUND) {
            printf("EXIT: FnType [%s] not found\n", fn_name);
            exit(0);
        }

        struct token_tmpvar *token = (struct token_tmpvar *)malloc(sizeof(struct token_tmpvar));
        token->token = TOKEN_TMPVAR;
        token->next = tn2->next;
        token->prev = tn1->prev->prev->prev->prev;
        token->next->prev = (struct token_node *)token;
        token->prev->next = (struct token_node *)token;
        token->tmpvar_id = dst;

        // Free operations
        tn1 = tn1->prev->prev->prev;
        tn2 = tn2->next;
        struct token_node *next = NULL;
        while (next != tn2) {
            next = tn1->next;
            free(tn1);
            tn1 = next;
        }

    } else {
        printf("EXIT: do1fncall: 1\n");
        exit(0);
    }
}

static void parse1line(struct parser_result *result, struct token_node *tn_start, struct token_node *tn_end, char *file_content) {

#ifdef DEBUG
    printf("---parse1line-1---\n");
    lex_dump(tn_start->next, tn_end, file_content);
#endif

    struct token_node *tn;
    struct token_node *tn1;
    struct token_node *tn2;
    char recheck_paranthesis;
    char is_fn_call;
    do {
        tn = tn_start->next;
        tn1 = NULL;
        tn2 = NULL;
        recheck_paranthesis = 0;
        while (tn != tn_end) {
            if (tn->token == TOKEN_LEFT_PARENTHESIS) {
                tn1 = tn;
            } else if (tn1 != NULL && tn->token == TOKEN_RIGHT_PARENTHESIS) {
                tn2 = tn;
                parse2paranthesis(result, tn1, tn2, file_content);

#ifdef DEBUG
                printf("---parse1line-2---\n");
                lex_dump(tn_start->next, tn_end, file_content);
#endif

                is_fn_call = tn1->prev->token == TOKEN_WORD;
                if (is_fn_call) {

                    if (tn1->next == tn2->prev) {
                        ;
                    } else if (tn1->next->next->token == TOKEN_COMMA && tn1->next->next == tn2->prev->prev) {
                        ;
                    } else {
                        printf("EXIT: parse1line: 1\n");
                        exit(0);
                    }

                    do1fncall(result, tn1, tn2, file_content);

                } else {

                    if (tn1->next != tn2->prev) {
                        printf("EXIT: parse1line: 2\n");
                        exit(0);
                    }

                    struct token_node *tn_center = tn1->next;
                    tn1->prev->next = tn_center;
                    tn2->next->prev = tn_center;
                    tn_center->next = tn2->next;
                    tn_center->prev = tn1->prev;

                    // Free operations
                    free(tn1);
                    free(tn2);
                }

#ifdef DEBUG
                printf("---parse1line-3---\n");
                lex_dump(tn_start->next, tn_end, file_content);
#endif

                recheck_paranthesis = 1;
                break;
            }
            tn = tn->next;
        }
    } while (recheck_paranthesis);

    tn = tn_start->next;
    if (tn->token == TOKEN_KEYWORD_LET && tn->next->token == TOKEN_WORD && tn->next->next->token == TOKEN_EQUALS) {

        parse2paranthesis(result, tn->next->next, tn_end, file_content);

#ifdef DEBUG
        printf("---parse1line-4---\n");
        lex_dump(tn_start->next, tn_end, file_content);
#endif

        if (tn->next->next->next->next->token == TOKEN_SEMICOLON) {

            uint32_t src1 = token_to_varid(result, tn->next->next->next, file_content);
            enum jsvar_type target_jstype = result->tmpvars[src1].js_type;
            uint32_t dst = register_uservar(result, NEW_USERVAR, file_content, ((struct token_span *)tn->next)->index, target_jstype);
            register_operation1(result, ASSIGN, src1, dst);

            // Free operations
            tn1 = tn_start->next;
            while(tn1 != tn_end){
                tn2 = tn1->next;
                free(tn1);
                tn1 = tn2;
            }

            tn_start->next = tn_end;

#ifdef DEBUG
            printf("---parse1line-5---\n");
            lex_dump(tn_start->next, tn_end, file_content);
#endif
        } else {
            printf("EXIT: parse1line: 3\n");
            exit(0);
        }
    }
}

struct parser_result *parse(struct readed_file *readed) {

    struct token_node *tn_start = readed->tn_start;
    struct token_node *tn_end = readed->tn_end;
    char *file_content = readed->file_content;

    hash_map = g_hash_table_new(g_str_hash, g_str_equal);

    map_static_fn_types = g_hash_table_new(g_str_hash, g_str_equal);

    const enum jsvar_type type_js_undefined = JS_UNDEFINED;
    const enum jsvar_type type_js_number = JS_NUMBER;
    const enum jsvar_type type_js_string = JS_STRING;
    g_hash_table_insert(map_static_fn_types, fn_console_log, (void *)&type_js_undefined);
    g_hash_table_insert(map_static_fn_types, fn_math_sqrt, (void *)&type_js_number);
    g_hash_table_insert(map_static_fn_types, fn_math_log2, (void *)&type_js_number);
    g_hash_table_insert(map_static_fn_types, fn_math_abs, (void *)&type_js_number);
    g_hash_table_insert(map_static_fn_types, fn_string_charat, (void *)&type_js_string);
    g_hash_table_insert(map_static_fn_types, fn_string_charcodeat, (void *)&type_js_number);
    g_hash_table_insert(map_static_fn_types, fn_string_substr, (void *)&type_js_string);

    struct parser_task0 *task_start = (struct parser_task0 *)malloc(sizeof(struct parser_task0));
    task_start->fn = START;
    task_start->next = NULL;

    struct parser_result *result = (struct parser_result *)malloc(sizeof(struct parser_result));
    result->task_start = task_start;
    result->task_end = task_start;
    result->tmpvars = (struct tmpvar_info *)malloc(sizeof(struct tmpvar_info) * 500);

    struct token_node *tn1 = tn_start->next;
    struct token_node *tn2 = tn1;
    while (tn2 != tn_end) {
        if (tn2->token == TOKEN_SEMICOLON) {
            parse1line(result, tn1->prev, tn2, file_content);
            tn1 = tn2->next;
        }
        tn2 = tn2->next;
    }

    // Free operations
    tn1 = tn_start;
    while (tn1->next != NULL) {
        tn2 = tn1->next;
        free(tn1);
        tn1 = tn2;
    }
    free(tn1);

    struct parser_task0 *task_end = (struct parser_task0 *)malloc(sizeof(struct parser_task0));
    task_end->fn = END;
    task_end->next = NULL;

    result->task_end->next = (struct parser_task0 *)task_end;
    result->task_end = task_end;

#ifdef DEBUG
    puts("---parser_dump---");
    parser_dump(result, file_content);
    puts("---parser_dump---");
#endif

    return result;
}
