//Hace parte del Ejercicio 9
//Compila con MaximoEnteroExe

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    int numeros[argc - 1];
    int maximo;
    char *endptr;

    errno = 0;
    for (int i = 1; i < argc; i++) {
        numeros[i - 1] = strtol(argv[i], &endptr, 10);
    }

    if(errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    
    maximo = numeros[0];
    for (int i = 1; i < argc - 1; i++) {
        if(numeros[i] > maximo) maximo = numeros[i];
    }
    
    printf("El maximo entero es: %d\n", maximo);

    exit(EXIT_SUCCESS);
}
