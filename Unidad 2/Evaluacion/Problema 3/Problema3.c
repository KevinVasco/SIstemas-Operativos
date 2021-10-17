#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>

#define BUF_SIZE 100

char name[10];

void* recibe_comando(void *parg) {

    /*
    *   Desactivacion Echo
    */
    struct termios tp;

    /* Retrieve current terminal settings, turn echoing off */

    if (tcgetattr(STDIN_FILENO, &tp) == -1)
        exit(EXIT_FAILURE);
    
    tp.c_lflag &= ~ECHO;                /* ECHO off, other bits unchanged */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1)
        exit(EXIT_FAILURE);

    while (1) {
        fgets(name, 10, stdin);
    }
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){

    FILE *inFile = fopen(argv[1],"r");
    if (inFile == NULL){
        perror("open file fails: ");
        exit(EXIT_FAILURE);
    }

    pthread_t threadID1;
    char buffer[64];
    char *status =  NULL;
    char upper[10] = "upper";
    char words[10] = "words";
    char vowels[10] = "vowels";
    char none[10] = "none";
    char *token;
    char sep[2] = " ";

    pthread_create(&threadID1,NULL,recibe_comando, NULL);
    
    do{
        status = fgets(buffer, sizeof(buffer),inFile);
        if(status != NULL){
            printf("%s",buffer);

            if (strcmp(name, upper) == 10) {
                int i = 0;
                char chr; 

                while (status[i]) { 
                    chr = status[i];
                    //if (chr == 160) chr = 193;        //  Esto es para convertir las tildes minusculas a mayusculas, pero no es soportado y no cambia.
                    //if (chr == 130) chr = 201;        //  Esto es para convertir las tildes minusculas a mayusculas, pero no es soportado y no cambia.
                    //if (chr == 161) chr = 205;        //  Esto es para convertir las tildes minusculas a mayusculas, pero no es soportado y no cambia.
                    //if (chr == 162) chr = 211;        //  Esto es para convertir las tildes minusculas a mayusculas, pero no es soportado y no cambia.
                    //if (chr == 163) chr = 218;        //  Esto es para convertir las tildes minusculas a mayusculas, pero no es soportado y no cambia.
                    printf("%c", toupper(chr)); 
                    i++; 
                }
            }

            if (strcmp(name, words) == 10) {
                int i = 0;
                token = strtok(status, sep);
                while (token != 0) {
                    token = strtok(NULL, sep);
                    i++;
                }
                printf("Words: %d\n", i);
            }

            if (strcmp(name, vowels) == 10) {
                int i = 0;
                int j = 0;
                char chr; 

                while (status[i]) { 
                    chr = status[i];
                    if (chr == 97) j++;
                    if (chr == 101) j++;
                    if (chr == 105) j++;
                    if (chr == 111) j++;
                    if (chr == 117) j++;
                    if (chr == 160) j++;
                    if (chr == 130) j++;
                    if (chr == 161) j++;
                    if (chr == 162) j++;
                    if (chr == 163) j++;
                    i++;
                }
                printf("Vowels: %d\n", j);
            }

            if (strcmp(name, none) == 10) {}          //  Implementacion por si se cambia la funcionalidad del comando <none>
            sleep(1);
        }
    }while (status !=NULL);

    printf("\n");
    fclose(inFile);

    exit(EXIT_SUCCESS);
}