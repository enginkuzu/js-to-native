#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct bigint {
    uint32_t array_length;
    uint32_t elements[];
};

struct bigint *mul(struct bigint *n1, struct bigint *n2){
    uint32_t array_length = n1->array_length + n2->array_length;
    struct bigint *ret = (struct bigint *)malloc(sizeof(uint32_t)*(array_length+1));
    ret->array_length = array_length;
    for(uint32_t i=0;i<array_length;i++){
        ret->elements[i] = 0;
    }
    for(uint32_t i=0;i<n1->array_length;i++){
        uint64_t v1 = n1->elements[i];
        for(uint32_t j=0;j<n2->array_length;j++){
            uint64_t v2 = n2->elements[j];
            uint64_t mul = v1 * v2;
            uint32_t l32 = mul & 0xFFFFFFFF;
            uint32_t h32 = mul >> 32;
            uint32_t index = i + j;

            uint64_t sum = l32 + ret->elements[index];
            ret->elements[index] = sum & 0xFFFFFFFF;

            sum = sum >> 32;
            index++;
            sum += h32 + ret->elements[index];
            ret->elements[index] = sum & 0xFFFFFFFF;

            while(1){
                sum = sum >> 32;
                if( sum == 0 ) break;
                index++;
                sum += ret->elements[index];
                ret->elements[index] = sum & 0xFFFFFFFF;
            }
        }
    }
    return ret;
}

void print(struct bigint *n){
    uint64_t rem;
    uint32_t index;
    uint32_t array_length = n->array_length;
    while(array_length > 0){
        index = array_length - 1;
        rem = 0;
        while(1){
            rem = (rem << 32) + n->elements[index];
            n->elements[index] = rem / 10;
            if( n->elements[index] == 0 && index == array_length - 1 ){
                array_length--;
            }
            rem %= 10;
            if( index == 0 ){
                printf("%c", '0'+(int8_t)rem);
                break;
            }
            index--;
        }
    }
    printf("\n");
}

int main(void) {

    struct bigint *n1 = (struct bigint *)malloc(sizeof(uint32_t)*(1+2));
    n1->array_length = 2;
    n1->elements[0] = 0xEB1F0AD2;
    n1->elements[1] = 0xAB54A98C;
    print(n1);
    n1->array_length = 2;
    n1->elements[0] = 0xEB1F0AD2;
    n1->elements[1] = 0xAB54A98C;

    struct bigint *n2 = (struct bigint *)malloc(sizeof(uint32_t)*(1+2));
    n2->array_length = 2;
    n2->elements[0] = 123456789;
    n2->elements[1] = 0;
    print(n2);
    n2->array_length = 2;
    n2->elements[0] = 123456789;
    n2->elements[1] = 0;

    struct bigint *n3 = mul(n1, n2);
    print(n3);
    free(n3);

    free(n2);
    free(n1);

    return 0;
}

