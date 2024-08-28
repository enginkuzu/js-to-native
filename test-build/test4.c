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
	JS_Number *var5 = new_number(6);
	JS_Number *var7 = new_number(0);
	JS_Number *var8 = new_number(var5->value + var7->value);
	JS_String *var9 = substr(var3, var8->value, var3->char_count);
	JS_String *var10 = var9;
	JS_Number *var12 = new_number(0);
	JS_Number *var14 = new_number(0);
	JS_Number *var15 = new_number(var12->value + var14->value);
	JS_Number *var17 = new_number(5);
	JS_Number *var19 = new_number(0);
	JS_Number *var20 = new_number(var17->value + var19->value);
	JS_String *var21 = substr(var3, var15->value, var20->value);
	JS_String *var22 = var21;
	print_string(var10->char_count, var10->text);
	print_string(var22->char_count, var22->text);
	JS_Number *var26 = new_number(0);
	JS_Number *var28 = new_number(13);
	JS_Number *var29 = new_number(var26->value - var28->value);
	JS_String *var30 = substr(var3, var29->value, var3->char_count);
	print_string(var30->char_count, var30->text);
	JS_Number *var33 = new_number(0);
	JS_Number *var35 = new_number(12);
	JS_Number *var36 = new_number(var33->value - var35->value);
	JS_String *var37 = substr(var3, var36->value, var3->char_count);
	print_string(var37->char_count, var37->text);
	JS_Number *var40 = new_number(0);
	JS_Number *var42 = new_number(11);
	JS_Number *var43 = new_number(var40->value - var42->value);
	JS_String *var44 = substr(var3, var43->value, var3->char_count);
	print_string(var44->char_count, var44->text);
	JS_Number *var47 = new_number(0);
	JS_Number *var49 = new_number(10);
	JS_Number *var50 = new_number(var47->value - var49->value);
	JS_String *var51 = substr(var3, var50->value, var3->char_count);
	print_string(var51->char_count, var51->text);
	JS_Number *var55 = new_number(0);
	JS_String *var53 = substr(var3, var55->value, var3->char_count);
	print_string(var53->char_count, var53->text);
	JS_Number *var59 = new_number(10);
	JS_String *var57 = substr(var3, var59->value, var3->char_count);
	print_string(var57->char_count, var57->text);
	JS_Number *var63 = new_number(11);
	JS_String *var61 = substr(var3, var63->value, var3->char_count);
	print_string(var61->char_count, var61->text);
	JS_Number *var67 = new_number(12);
	JS_String *var65 = substr(var3, var67->value, var3->char_count);
	print_string(var65->char_count, var65->text);
	JS_Number *var71 = new_number(13);
	JS_String *var69 = substr(var3, var71->value, var3->char_count);
	print_string(var69->char_count, var69->text);
}
