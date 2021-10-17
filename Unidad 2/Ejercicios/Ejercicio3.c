#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    pid_t pid_hijo; //Una variable que guardara la id de proceso de alguien

    printf("Este proceso va a crear otro proceso\n\n");
    printf("El pid del programa padre es: %d\n", (int)getpid());
    pid_hijo = fork();
    switch(pid_hijo) {
        case -1: /* Error */
            printf("Eror al crear el proceso");
        return -1;
        case 0: /*Codigo ejecutado por el hijo*/
            printf("PROCESO HIJO:\n");
            printf("Soy el hijo, mi PID es %d\n", (int)getpid());
        break;
        default: /* Codigo ejecutado por el padre*/
            printf("PROCESO PADRE: Proceso hijo con PID %d creado\n", (int)pid_hijo);
    }
    /* Esta linea sera ejecutada por ambos procesos, pero en
        un diferente contexto (el de cada proceso) */
    printf("Soy el proceso %d terminado \n", (int)getpid());
    exit(EXIT_SUCCESS);
    printf("No deberia estar aqui");
    //Vainas raras de la terminal
}
