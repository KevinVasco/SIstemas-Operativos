#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char const *argv[]) {
    // ./exe 1 2 3 4 5
    pid_t pid_Hijo_1;
    pid_t pid_Hijo_2;
    pid_t pid_Hijo_3;

    pid_Hijo_1 = fork();
    switch(pid_Hijo_1) {
    case -1:
        perror("Error al crear el primer hijo!\n");
        exit(EXIT_FAILURE);
    case 0:
        argv[0] = "./MaximoEnteroExe";
        execv(argv[0], argv);
        break;
    default:
    pid_Hijo_2 = fork();
        switch (pid_Hijo_2) {
        case -1:
            perror("Error al crear el segundo hijo!\n");
            exit(EXIT_FAILURE);
        case 0:
            argv[0] = "./MinimoEnteroExe";
            execv(argv[0], argv);
            break;
        default:
        pid_Hijo_3 = fork();
            switch (pid_Hijo_3) {
            case -1:
                perror("Error al crear el tercer hijo!\n");
                exit(EXIT_FAILURE);
            case 0:
                argv[0] = "./PromedioExe";
                execv(argv[0], argv);
                break;
            default:
                waitpid(pid_Hijo_1, NULL, 0);
                waitpid(pid_Hijo_2, NULL, 0);
                waitpid(pid_Hijo_3, NULL, 0);
                printf("Soy el padre. Ya se termino de ejecutar todo correctamente.\n");
                break;
            }
            break;
        }
        break;
    }
    exit(EXIT_SUCCESS);
}
