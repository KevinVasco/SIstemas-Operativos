//Ejercicio 16
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct personas {
    char *nombre;
    char *ocupacion;
    int edad;
};

int main(int argc, char const *argv[]) {
    /*
    * Abro el archivo
    */
    FILE *inFile = fopen(argv[1],"r");
    if (inFile == NULL){
        perror("open file fails: ");
        return(EXIT_FAILURE);
    }

    char buffer[20];
    char *status =  NULL;
    struct personas *lista;
    char *token;
    char sep[2] = " ";
    int count = 0;
    char *endbuffer;
    do{
        /*
        * status contendra la primera linea hasta la n-esima linea
        */
        status = fgets(buffer, sizeof(buffer),inFile);
        if(status != NULL){
            printf("Satus: %s\n", status);
            lista = malloc(sizeof(struct personas) * 100);
            token = strtok(status, sep);
            lista[count].nombre = token;
            token = strtok(NULL, sep);
            lista[count].ocupacion = token;
            //lista[count].edad = strtol(token, &endbuffer, 10);
            printf("Name: %s, Occupation: %s, Age: %d\n", lista[count].nombre, lista[count].ocupacion, lista[count].edad);
        }
    }while (status !=NULL);

    printf("\n");

    fclose(inFile);
    exit(EXIT_SUCCESS);
}
