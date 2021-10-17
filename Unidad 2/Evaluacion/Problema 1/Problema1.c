#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct IntArray{
    int *parr;
    int length;
}IntArray;


IntArray *createIntArray(int size);
void initIntArray(IntArray *);
void destroyIntArray(IntArray *);
void printArray(IntArray *);

int main(){
    IntArray * pIntArray = createIntArray(100);
    initIntArray(pIntArray);
    printArray(pIntArray);
    destroyIntArray(pIntArray);
}

IntArray *createIntArray(int size){
        IntArray *pIntArray = malloc(sizeof(IntArray));
        pIntArray->parr = malloc( sizeof(int) * size);
        pIntArray->length = size;
        return pIntArray;
}

void initIntArray(IntArray *this){
    for(int i = 0; i < 100; i++){
        this->parr[i] = i;
    }
}

void printArray(IntArray *this){
    for(int i = 0; i < this->length; i++){
        printf("parr[%d]: %d\n", i, this->parr[i]);
    }
}

void destroyIntArray(IntArray *this){
    free(this->parr);
    free(this);
}