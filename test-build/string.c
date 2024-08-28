#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

typedef enum {
    JS_TYPE_STRING,
    JS_TYPE_NUMBER,
    JS_TYPE_BOOLEAN,
    JS_TYPE_UNDEFINED,
    JS_TYPE_NULL,
    JS_TYPE_BIGINT,
    JS_TYPE_OBJECT,
    JS_TYPE_ARRAY,
    JS_TYPE_DATE,
} JS_TYPE;

typedef struct {
	JS_TYPE js_type;
	uint32_t char_count;
        char16_t *text;
} JS_String;

typedef struct {
    JS_TYPE js_type;
    double value;
} JS_Number;

typedef struct {
    JS_TYPE js_type;
    char value;
} JS_Boolean;

typedef struct {
    JS_TYPE js_type;
} JS_Undefined;

typedef struct {
    JS_TYPE js_type;
} JS_Null;

typedef struct {
    JS_TYPE js_type;
    // TODO : fill
} JS_Bigint;

typedef struct {
    JS_TYPE js_type;
    // TODO : fill
} JS_Object;

typedef struct {
    JS_TYPE js_type;
    // TODO : fill
} JS_Array;

typedef struct {
    JS_TYPE js_type;
    // TODO : fill
} JS_Date;

static JS_String VALUE_EMPTY_STRING = {JS_TYPE_STRING, 0, u""};
static JS_Number VALUE_NAN = {JS_TYPE_NUMBER, NAN};
static JS_Boolean VALUE_TRUE = {JS_TYPE_BOOLEAN, 1};
static JS_Boolean VALUE_FALSE = {JS_TYPE_BOOLEAN, 0};
static JS_Undefined VALUE_UNDEFINED = {JS_TYPE_UNDEFINED};
static JS_Null VALUE_NULL = {JS_TYPE_NULL};

JS_String *new_string(char16_t *text, uint32_t length) {
    JS_String *ret = (JS_String *)malloc(sizeof(JS_String));
    ret->js_type = JS_TYPE_STRING;
    ret->char_count = length;
    ret->text = text;
    return ret;
}

JS_Number *new_number(double value) {
    JS_Number *ret = (JS_Number *)malloc(sizeof(JS_Number));
    ret->js_type = JS_TYPE_NUMBER;
    ret->value = value;
    return ret;
}

void print_double(double value) {
    if (isnan(value)) {
        printf("NaN\n");
    } else {
        printf("%.16g\n", value);
    }
}

void print_string(int length, char16_t *text) {
    uint32_t tmp_len = 0;
    char *tmp = (char *)malloc(length * 3);
    for (int i = 0; i < length; i++) {
        char16_t ch = text[i];
        if (ch < 0x007F) {
            tmp[tmp_len++] = ch;
        } else if (ch < 0x07FF) {
            tmp[tmp_len++] = 0xC0 | (ch >> 6);
            tmp[tmp_len++] = 0x80 | (ch & 0x3F);
        } else {
            tmp[tmp_len++] = 0xE0 | (ch >> 12);
            tmp[tmp_len++] = 0x80 | ((ch >> 6) & 0x3F);
            tmp[tmp_len++] = 0x80 | (ch & 0x3F);
        }
    }
    printf("%.*s\n", tmp_len, tmp);
    free(tmp);
}

JS_String *string_append_string(JS_String *param1, JS_String *param2) {
    uint32_t length = param1->char_count + param2->char_count;
    char16_t *buffer = (char16_t *)malloc(length << 1);
    memcpy(buffer, param1->text, param1->char_count << 1);
    memcpy(buffer + param1->char_count, param2->text, param2->char_count << 1);
    return new_string(buffer, length);
}

JS_String *string_append_number(JS_String *param1, double param2) {
    char *tmp_buffer = (char *)malloc(50);
    int double_count = sprintf(tmp_buffer, "%g", param2);
    uint32_t length = param1->char_count + double_count;
    char16_t *buffer = (char16_t *)malloc(length << 1);
    memcpy(buffer, param1->text, param1->char_count << 1);
    for (int i = 0; i < double_count; i++) {
        buffer[param1->char_count + i] = tmp_buffer[i];
    }
    free(tmp_buffer);
    return new_string(buffer, length);
}

JS_String *number_append_string(double param1, JS_String *param2) {
    char *tmp_buffer = (char *)malloc(50);
    int double_count = sprintf(tmp_buffer, "%g", param1);
    uint32_t length = double_count + param2->char_count;
    char16_t *buffer = (char16_t *)malloc(length << 1);
    for (int i = 0; i < double_count; i++) {
        buffer[i] = tmp_buffer[i];
    }
    memcpy(buffer + double_count, param2->text, param2->char_count << 1);
    free(tmp_buffer);
    return new_string(buffer, length);
}

JS_String *charat(JS_String *str, int index) {
    int text_len = str->char_count;
    if (index >= text_len)
        return &VALUE_EMPTY_STRING;
    if( index < 0 ){
        index += text_len;
        if( index < 0 ) return &VALUE_EMPTY_STRING;
    }
    JS_String *ret = new_string(str->text + index, 1);
    return ret;
}

JS_Number *charcodeat(JS_String *str, int index) {
    int text_len = str->char_count;
    if (index >= text_len)
        return &VALUE_NAN;
    if( index < 0 ){
        index += text_len;
        if( index < 0 ) return &VALUE_NAN;
    }
    JS_Number *ret = new_number(str->text[index]);
    return ret;
}

JS_String *substr(JS_String *str, int from, int length) {
    int text_len = str->char_count;
    if (from >= text_len)
        return &VALUE_EMPTY_STRING;
    if( from < 0 ){
        from += text_len;
        if( from < 0 ) from = 0;
    }
    if (length <= 0)
        return &VALUE_EMPTY_STRING;
    if( length > text_len - from ) length = text_len - from;
    JS_String *ret = new_string(str->text + from, length);
    return ret;
}

int main(void){
	JS_String *var2 = new_string(u"Hello world!", (sizeof(u"Hello world!") >> 1) - 1);
	JS_String *var3 = var2;
	print_string(var3->char_count, var3->text);
	JS_Number *var7 = new_number(0);
	JS_String *var5 = charat(var3, var7->value);
	print_string(var5->char_count, var5->text);
	JS_Number *var11 = new_number(11);
	JS_String *var9 = charat(var3, var11->value);
	print_string(var9->char_count, var9->text);
	JS_Number *var15 = new_number(12);
	JS_String *var13 = charat(var3, var15->value);
	print_string(var13->char_count, var13->text);
	JS_Number *var19 = new_number(0);
	JS_Number *var17 = charcodeat(var3, var19->value);
	print_double(var17->value);
	JS_Number *var23 = new_number(11);
	JS_Number *var21 = charcodeat(var3, var23->value);
	print_double(var21->value);
	JS_Number *var27 = new_number(12);
	JS_Number *var25 = charcodeat(var3, var27->value);
	print_double(var25->value);
	JS_String *var30 = new_string(u"a܍☺܍a", (sizeof(u"a܍☺܍a") >> 1) - 1);
	JS_String *var31 = var30;
	JS_Number *var34 = new_number(3);
	JS_String *var32 = charat(var31, var34->value);
	print_string(var32->char_count, var32->text);
	JS_Number *var38 = new_number(3);
	JS_Number *var36 = charcodeat(var31, var38->value);
	print_double(var36->value);
	JS_Number *var42 = new_number(3);
	JS_Number *var44 = new_number(1);
	JS_String *var40 = substr(var31, var42->value, var44->value);
	print_string(var40->char_count, var40->text);
}
