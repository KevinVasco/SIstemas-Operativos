//Hace parte del Ejercicio 9
//Compila con PromedioExe

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    int numeros[argc - 1];
    float resultado = 0;
    char *endptr;

    errno = 0;
    for (int i = 1; i < argc; i++) {
        numeros[i - 1] = strtol(argv[i], &endptr, 10);
        resultado += numeros[i - 1];
    }

    if(errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    resultado /= (argc - 1);
    printf("El promedio es: %f\n", resultado);
    
    exit(EXIT_SUCCESS);
}
