
#include <stdio.h>
#include <string.h>
#include <uchar.h>
/*
First code point	Last code point	Byte 1	Byte 2	Byte 3	Byte 4
U+0000	    U+007F	    0xxxxxxx	
U+0080	    U+07FF	    110xxxxx	10xxxxxx	
U+0800	    U+FFFF	    1110xxxx	10xxxxxx	10xxxxxx	
U+010000	[b]U+10FFFF	11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
*/

char tmp[4];

void utf16_to_utf8(char16_t ch){
    if( ch < 0x007F ){
        tmp[0] = ch;
        tmp[1] = 0;
    } else if( ch < 0x07FF ){
        tmp[0] = 0xC0 | (ch >> 6);
        tmp[1] = 0x80 | (ch & 0x3F);
        tmp[2] = 0;
    } else {
        tmp[0] = 0xE0 | (ch >> 12);
        tmp[1] = 0x80 | ((ch >> 6) & 0x3F);
        tmp[2] = 0x80 | (ch & 0x3F);
        tmp[3] = 0;
    }
}

int main(void)
{
    // 0x61, 0xDC 0x8D, 0xE2 0x98 0xBA, 0x00
    char utf8[] = "a܍☺";
    //    0x0061,    0x070D,    0x263A,    0x0000
    // 0x61 0x00, 0x0D 0x07, 0x3A 0x26, 0x00 0x00
    char16_t utf16[] = u"a܍☺";

    char *src;
    src = (char *)utf8;
    for(int i=0;i<10;i++){
        printf("%02X ", src[i]);
    }
    printf("\n%lu byte with terminator\n", sizeof(utf8));
    src = (char *)utf16;
    for(int i=0;i<10;i++){
        printf("%02X ", src[i]);
    }
    printf("\n%lu byte with terminator\n", sizeof(utf16));

    printf("%s\n", utf8);
    
    utf16_to_utf8(utf16[0]);
    printf("%s\n", tmp);
    utf16_to_utf8(utf16[1]);
    printf("%s\n", tmp);
    utf16_to_utf8(utf16[2]);
    printf("%s\n", tmp);

    return 0;
}

