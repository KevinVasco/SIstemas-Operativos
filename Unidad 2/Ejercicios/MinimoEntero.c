//Hace parte del Ejercicio 9
//Compila con MaximoEnteroExe

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    int numeros[argc - 1];
    int minimo;
    char *endptr;

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
    
    printf("El minimo entero es: %d\n", minimo);

    exit(EXIT_SUCCESS);
}
