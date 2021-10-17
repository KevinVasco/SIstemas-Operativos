#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 10

int main(int argc, char *argv[])
{
    int pfd[2]; /* Pipe file descriptors */
    char buf[BUF_SIZE];
    ssize_t numRead;

    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("usage error: %s string\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    if (pipe(pfd) == -1){ /* Create the pipe */
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch (fork())
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0: /* Child  - reads from pipe */
        if (close(pfd[1]) == -1)
        { /* Write end is unused */
            perror("close - child");
            exit(EXIT_FAILURE);
        }
        for (;;)
        { /* Read data from pipe, echo on stdout */
            numRead = read(pfd[0], buf, BUF_SIZE);
            if (numRead == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }

            if (numRead == 0)
                break; /* End-of-file */
            if (write(STDOUT_FILENO, buf, numRead) != numRead)
            {
                perror("child - partial/failed write");
                exit(EXIT_FAILURE);
            }
        }
        write(STDOUT_FILENO, "\n", 1);
        if (close(pfd[0]) == -1)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    default: /* Parent - writes to pipe */
        if (close(pfd[0]) == -1)
        { /* Read end is unused */
            perror("close - parent");
            exit(EXIT_FAILURE);
        }

        if (write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
        {
            perror("parent - partial/failed write");
            exit(EXIT_FAILURE);
        }
        if (close(pfd[1]) == -1)
        { /* Child will see EOF */
            perror("close");
            exit(EXIT_FAILURE);
        }
        wait(NULL); /* Wait for child to finish */
        exit(EXIT_SUCCESS);
    }
}