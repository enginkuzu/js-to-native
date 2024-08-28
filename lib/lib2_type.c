
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
