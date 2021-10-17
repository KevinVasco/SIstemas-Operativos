#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* imprime_x(void *param){
    for (int i = 0; i < 1000; i++) {
        printf("x");
    }
    printf("\n");
    return NULL;
}


int main(int argc, char *argv[]){
    pthread_t threadID;
    pthread_create(&threadID,NULL,&imprime_x,NULL);
    for (int i = 0; i < 1000; i++) {
        printf("o");
    }
    //pthread_join(threadID, NULL);
    printf("\n");
    exit(EXIT_SUCCESS);
}