#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char const *argv[]) {
    srand(time(NULL));   // Initialization, should only be called once.

    FILE *archv = fopen("./texto_random", "a+");

    if (archv == NULL) {
        perror("Open file fin fails: ");
        return(EXIT_FAILURE);
    }

    for (int i = 0; i < 100; i++) {
        int a = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX.
        int b = rand();      
        int c = rand();      

        a = a % 10 + 1;
        b = b % 10 + 1;
        c = c % 10 + 1;


        for (int i = 0; i < 6; i++) {
            fprintf(archv, "%s", "a");
        }
        fprintf(archv, "%s", " ");

        for (int i = 0; i < 7; i++) {
            fprintf(archv, "%s", "b");
        }
        fprintf(archv, "%s", " ");

        fprintf(archv, "%d", c * 7);
        
        if(i != 99) fprintf(archv, "\n");
    }

    exit(EXIT_SUCCESS);
}
