
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
