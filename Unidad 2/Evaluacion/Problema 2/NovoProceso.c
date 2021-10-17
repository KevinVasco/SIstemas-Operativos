//  Hace parte de Problema 2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {

    char *endptr;
    int id_hijo = strtol(argv[1], &endptr, 10);
    waitpid(id_hijo, NULL, 0);


    FILE *inFile = fopen("./OldSKol.txt","r");
    if (inFile == NULL){
        perror("open file fails: ");
        return(EXIT_FAILURE);
    }

    char buffer[64];
    char *status =  NULL;
    int resultado = 99;

    status = fgets(buffer, sizeof(buffer),inFile);
    if(status != NULL){
        resultado = strtol(status, &endptr, 10);
    }
    printf("El resultado es: %d\n", resultado);

    fclose(inFile);

    exit(EXIT_SUCCESS);
}