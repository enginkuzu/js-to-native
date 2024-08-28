#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "5save.h"
#include "9global.h"
#include "js_function.h"
#include "test-build/lib1_header.c"
#include "test-build/lib2_type.c"
#include "test-build/lib3_function.c"

static void save_file_content(char *file_name, char *buffer, int bindex) {
    FILE *fp = fopen(file_name, "w");
    fwrite(buffer, bindex, 1, fp);
    fclose(fp);
}

static int bindex;
static char *buffer;

void do_dst(struct parser_result *parsed, uint32_t dst) {
    struct tmpvar_info *info_dst = parsed->tmpvars + dst;
    if (info_dst->js_type == JS_NUMBER) {
        bindex += sprintf(buffer + bindex, "\tJS_Number *var%d = ", dst);
    } else if (info_dst->js_type == JS_STRING) {
        bindex += sprintf(buffer + bindex, "\tJS_String *var%d = ", dst);
    } else {
        printf("EXIT: Unknown JS Type : %d\n", info_dst->js_type);
        exit(0);
    }
}

void do_src(struct parser_result *parsed, char *file_content, uint32_t src) {
    struct tmpvar_info *info_src = parsed->tmpvars + src;
    switch (info_src->type) {
    case NEW_CONST:
        if (info_src->js_type == JS_NUMBER) {
            bindex += sprintf(buffer + bindex, "new_number(%s)", file_content + info_src->index);
        } else if (info_src->js_type == JS_STRING) {
            bindex += sprintf(buffer + bindex, "new_string(u\"%s\", (sizeof(u\"%s\") >> 1) - 1)", file_content + info_src->index, file_content + info_src->index);
        } else {
            printf("EXIT: Unknown JS Type : %d\n", info_src->js_type);
            exit(0);
        }
        break;
    case NEW_TMPVAR:
    case NEW_USERVAR:
        bindex += sprintf(buffer + bindex, "var%d", info_src->tmpvar_id);
        break;
    default:
        printf("EXIT: UnknownTmpVarType:%d\n", info_src->type);
        exit(0);
        break;
    }
}

void save(char *file_name, struct parser_result *parsed, char *file_content) {

    bindex = 0;
    buffer = (char *)malloc(64 * 1024);

    struct parser_task0 *task = parsed->task_start;
    while (task != NULL) {
        switch (task->fn) {
        case START:
            strncpy(buffer + bindex, (const char *)lib_lib1_header_c, lib_lib1_header_c_len);
            bindex += lib_lib1_header_c_len;
            strncpy(buffer + bindex, (const char *)lib_lib2_type_c, lib_lib2_type_c_len);
            bindex += lib_lib2_type_c_len;
            strncpy(buffer + bindex, (const char *)lib_lib3_function_c, lib_lib3_function_c_len);
            bindex += lib_lib3_function_c_len;
            bindex += sprintf(buffer + bindex, "\n");
            bindex += sprintf(buffer + bindex, "int main(void){\n");
            break;
        case END:
            bindex += sprintf(buffer + bindex, "}\n");
            break;
        case ASSIGN: {
            struct parser_task1 *task1 = (struct parser_task1 *)task;
            do_dst(parsed, task1->dst);
            do_src(parsed, file_content, task1->src);
            bindex += sprintf(buffer + bindex, ";\n");
            break;
        }
        case FUNCTION_CALL: {
            struct parser_task2 *task2 = (struct parser_task2 *)task;
            if (strcmp(task2->fn_name, fn_console_log) == 0) {
                struct tmpvar_info *info_src = parsed->tmpvars + task2->src1;
                switch (info_src->type) {
                case NEW_CONST:
                    if (info_src->js_type == JS_NUMBER) {
                        bindex += sprintf(buffer + bindex, "\tprint_double(%s->value);\n", file_content + info_src->index);
                    } else if (info_src->js_type == JS_STRING) {
                        bindex += sprintf(buffer + bindex, "\tprint_string(%s->char_count, %s->text);\n", file_content + info_src->index, file_content + info_src->index);
                    } else {
                        printf("EXIT: Unknown JS Type : %d\n", info_src->js_type);
                        exit(0);
                    }
                    break;
                case NEW_TMPVAR:
                case NEW_USERVAR:
                    if (info_src->js_type == JS_NUMBER) {
                        bindex += sprintf(buffer + bindex, "\tprint_double(var%d->value);\n", info_src->tmpvar_id);
                    } else if (info_src->js_type == JS_STRING) {
                        bindex += sprintf(buffer + bindex, "\tprint_string(var%d->char_count, var%d->text);\n", info_src->tmpvar_id, info_src->tmpvar_id);
                    } else {
                        printf("EXIT: Unknown JS Type : %d\n", info_src->js_type);
                        exit(0);
                    }
                    break;
                default:
                    printf("EXIT: UnknownTmpVarType:%d\n", info_src->type);
                    exit(0);
                    break;
                }
            } else if (strcmp(task2->fn_name, fn_math_sqrt) == 0) {
                do_dst(parsed, task2->dst);
                bindex += sprintf(buffer + bindex, "new_number(sqrt(");
                do_src(parsed, file_content, task2->src1);
                bindex += sprintf(buffer + bindex, "->value");
                bindex += sprintf(buffer + bindex, "));\n");
            } else if (strcmp(task2->fn_name, fn_math_log2) == 0) {
                do_dst(parsed, task2->dst);
                bindex += sprintf(buffer + bindex, "new_number(log2(");
                do_src(parsed, file_content, task2->src1);
                bindex += sprintf(buffer + bindex, "->value");
                bindex += sprintf(buffer + bindex, "));\n");
            } else if (strcmp(task2->fn_name, fn_math_abs) == 0) {
                do_dst(parsed, task2->dst);
                bindex += sprintf(buffer + bindex, "new_number(fabs(");
                do_src(parsed, file_content, task2->src1);
                bindex += sprintf(buffer + bindex, "->value");
                bindex += sprintf(buffer + bindex, "));\n");
            } else if (strcmp(task2->fn_name, fn_string_charat) == 0) {
                do_dst(parsed, task2->dst);
                bindex += sprintf(buffer + bindex, "charat(");
                do_src(parsed, file_content, task2->src1);
                bindex += sprintf(buffer + bindex, ", ");
                do_src(parsed, file_content, task2->src2);
                bindex += sprintf(buffer + bindex, "->value");
                bindex += sprintf(buffer + bindex, ");\n");
            } else if (strcmp(task2->fn_name, fn_string_charcodeat) == 0) {
                do_dst(parsed, task2->dst);
                bindex += sprintf(buffer + bindex, "charcodeat(");
                do_src(parsed, file_content, task2->src1);
                bindex += sprintf(buffer + bindex, ", ");
                do_src(parsed, file_content, task2->src2);
                bindex += sprintf(buffer + bindex, "->value");
                bindex += sprintf(buffer + bindex, ");\n");
            } else if (strcmp(task2->fn_name, fn_string_substr) == 0) {
                do_dst(parsed, task2->dst);
                bindex += sprintf(buffer + bindex, "substr(");
                do_src(parsed, file_content, task2->src1);
                bindex += sprintf(buffer + bindex, ", ");
                do_src(parsed, file_content, task2->src2);
                bindex += sprintf(buffer + bindex, "->value");
                bindex += sprintf(buffer + bindex, ", ");
                if (task2->src3 == 0) {
                    do_src(parsed, file_content, task2->src1);
                    bindex += sprintf(buffer + bindex, "->char_count");
                } else {
                    do_src(parsed, file_content, task2->src3);
                    bindex += sprintf(buffer + bindex, "->value");
                }
                bindex += sprintf(buffer + bindex, ");\n");
            } else {
                printf("EXIT: Unknown Function Call : %d\n", task->fn);
                exit(0);
            }

            // Free operation
            free(task2->fn_name);

            break;
        }
        case DOUBLE_ADD:
        case DOUBLE_SUB:
        case DOUBLE_MUL:
        case DOUBLE_DIV: {
            struct parser_task3 *task3 = (struct parser_task3 *)task;
            bindex += sprintf(buffer + bindex, "\tJS_Number *var%d = new_number(", task3->dst);
            do_src(parsed, file_content, task3->src1);
            bindex += sprintf(buffer + bindex, "->value");
            if (task->fn == DOUBLE_ADD) {
                bindex += sprintf(buffer + bindex, " + ");
            } else if (task->fn == DOUBLE_SUB) {
                bindex += sprintf(buffer + bindex, " - ");
            } else if (task->fn == DOUBLE_MUL) {
                bindex += sprintf(buffer + bindex, " * ");
            } else {
                bindex += sprintf(buffer + bindex, " / ");
            }
            do_src(parsed, file_content, task3->src2);
            bindex += sprintf(buffer + bindex, "->value");
            bindex += sprintf(buffer + bindex, ");\n");
            break;
        }
        case STRING_ADD: {
            struct parser_task3 *task3 = (struct parser_task3 *)task;
            struct tmpvar_info *info_src1;
            struct tmpvar_info *info_src2;
            {
                info_src1 = parsed->tmpvars + task3->src1;
                if (info_src1->type == NEW_TMPVAR || info_src1->type == NEW_USERVAR) {
                    ;
                } else {
                    printf("EXIT: Unexpected TmpVar Type %d\n", info_src1->type);
                    exit(0);
                }
            }
            {
                info_src2 = parsed->tmpvars + task3->src2;
                if (info_src2->type == NEW_TMPVAR || info_src2->type == NEW_USERVAR) {
                    ;
                } else {
                    printf("EXIT: Unexpected TmpVar Type %d\n", info_src2->type);
                    exit(0);
                }
            }

            if (info_src1->js_type == JS_STRING && info_src2->js_type == JS_STRING) {
                bindex += sprintf(buffer + bindex, "\tJS_String *var%d = string_append_string(var%d, var%d);\n", task3->dst, info_src1->tmpvar_id, info_src2->tmpvar_id);
            } else if (info_src1->js_type == JS_STRING && info_src2->js_type == JS_NUMBER) {
                bindex += sprintf(buffer + bindex, "\tJS_String *var%d = string_append_number(var%d, var%d->value);\n", task3->dst, info_src1->tmpvar_id, info_src2->tmpvar_id);
            } else if (info_src1->js_type == JS_NUMBER && info_src2->js_type == JS_STRING) {
                bindex += sprintf(buffer + bindex, "\tJS_String *var%d = number_append_string(var%d->value, var%d);\n", task3->dst, info_src1->tmpvar_id, info_src2->tmpvar_id);
            } else {
                printf("EXIT: TODO51\n");
                exit(0);
            }

            break;
        }
        default:
            printf("EXIT: UnknownFn:%d\n", task->fn);
            exit(0);
            break;
        }
        task = task->next;
    }

    // Free operations
    task = parsed->task_start;
    struct parser_task0 *task_next;
    while (task != NULL) {
        task_next = task->next;
        free(task);
        task = task_next;
    }

#ifdef DEBUG
    printf("---save---\n");
    printf("%s", buffer);
    printf("---save---\n");
#endif

    save_file_content(file_name, buffer, bindex);

    // Free operations
    free(buffer);
};
