#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
    pid_t pid_hijo_mayor;
    pid_t pid_hijo_menor;
    int n = 11;
    int resultado = 1;

    pid_hijo_mayor = fork();
    switch (pid_hijo_mayor) {
    case -1:
        printf("Error al crear el proceso!");
        return -1;
    case 0:
        //Hijo Mayor
        printf("Soy el hijo mayor con id: %d\n", (int)getpid());
        for (int i = 1; i < n; i++) {
            for (int j = i; j > 1; j--) {
                resultado *= j;
            }
            printf("Hijo: %d: fact(%d) = %d\n", (int)getpid(), i, resultado);
            resultado = 1;
        }
        break;
    default:
        pid_hijo_menor = fork();
        switch (pid_hijo_menor) {
        case -1:
            printf("Error al crear al hijo menor!");
            return -1;
        case 0:
            //Hijo Menor
            printf("Soy el hijo menor con id: %d\n", (int)getpid());
            for (int i = 1; i < n; i++) {
                for (int j = i; j > 1; j--) {
                    resultado *= j;
                }
                printf("Hijo: %d: fact(%d) = %d\n", (int)getpid(), i, resultado);
                resultado = 1;
            }
            break;
        default:
            //Padre
            waitpid(pid_hijo_mayor, NULL, 0);
            waitpid(pid_hijo_menor, NULL, 0);
            printf("Padre");
            for (int i = 1; i < n; i++) {
                for (int j = i; j > 1; j--) {
                    resultado *= j;
                }
                printf("Padre: %d: fact(%d) = %d\n", (int)getpid(), i, resultado);
                resultado = 1;
            }
            break;
        }
    }
    return 0;
}
