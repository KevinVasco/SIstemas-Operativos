#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char const *argv[]) {

    pid_t pid_hijo;
    FILE *inFile = fopen("./OldSKol.txt","w+");
        if (inFile == NULL){
            perror("open file fails: ");
            return(EXIT_FAILURE);
        }
    //  Eso por FIFO no me da confianza adentrar.
    //  Pasar por arg el pid del hijo
    //  Cambiar Switch por For || Switch da muchos problemas para las instancias

    pid_hijo = fork();
    if (pid_hijo == -1) {
        perror("Error al crear segundo proceso!\n");
        exit(EXIT_FAILURE);
    }

    if (pid_hijo == 0) {
        char *endptr;
        int numeros[argc - 1];
        int minimo;

        errno = 0;
        for (int i = 1; i < argc; i++) {
            numeros[i - 1] = strtol(argv[i], &endptr, 10);
        }

        if(errno != 0) {
            perror("strtol");
            exit(EXIT_FAILURE);
        }

        minimo = numeros[0];
        for (int i = 1; i < argc - 1; i++) {
            if(numeros[i] < minimo) minimo = numeros[i];
        }

        fprintf(inFile, "%d", minimo);
    }

    else {
        char str[64];
        sprintf(str, "%d", pid_hijo);
        char *args[] = {"./NovoProceso", str, NULL};
        execv(args[0],args);
    }

    exit(EXIT_SUCCESS);
}
