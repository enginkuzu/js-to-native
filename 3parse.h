#pragma once

#include "2lex.h"

enum parser_fn {
    START,
    END,
    ASSIGN,
    FUNCTION_CALL,
    DOUBLE_ADD,
    DOUBLE_SUB,
    DOUBLE_MUL,
    DOUBLE_DIV,
    STRING_ADD,
};

// START
// END
struct parser_task0 {
    enum parser_fn fn;
    struct parser_task0 *next;
};

// ASSIGN
struct parser_task1 {
    enum parser_fn fn;
    struct parser_task0 *next;
    uint32_t src;
    uint32_t dst;
};

// FUNCTION_CALL
struct parser_task2 {
    enum parser_fn fn;
    struct parser_task0 *next;
    char *fn_name;
    uint32_t src1;
    uint32_t src2;
    uint32_t src3;
    uint32_t dst;
};

// DOUBLE_ADD
// DOUBLE_SUB
// DOUBLE_MUL
// DOUBLE_DIV
// STRING_ADD
struct parser_task3 {
    enum parser_fn fn;
    struct parser_task0 *next;
    uint32_t src1;
    uint32_t src2;
    uint32_t dst;
};

enum tmpvar_type {
    NEW_CONST,
    NEW_TMPVAR,
    NEW_USERVAR,
};

enum jsvar_type {
    JS_TYPE_NOT_FOUND,
    JS_UNDEFINED,
    JS_NUMBER,
    JS_STRING,
};

struct tmpvar_info {
    enum tmpvar_type type;
    long index;
    uint32_t tmpvar_id;
    enum jsvar_type js_type;
};

struct parser_result {
    struct parser_task0 *task_start;
    struct parser_task0 *task_end;
    struct tmpvar_info *tmpvars;
};

struct parser_result *parse(struct readed_file *readed);
