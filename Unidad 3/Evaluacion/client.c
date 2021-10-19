#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define SERVER_KEY_PATHNAME "./server_key_path.txt"
#define PROJECT_ID 'M'
#define SIZE_COMMAND 256

typedef struct message_text {
    int qid;
    char buf [SIZE_COMMAND];
}message_text;

typedef struct message {
    long message_type;
    message_text message_text;
}message;

key_t server_queue_key;
int server_qid, my_qid;

void *server_listener_closing (void *pargs) {
    message server_message;
    if (msgrcv (my_qid, &server_message, sizeof (message), 2, 0) == -1) {
        perror ("client: msgrcv\n");
        printf ("client: server tried to close\n");
        printf ("client: closing client\n");
        exit (EXIT_FAILURE);
    }
    printf ("server: closing\n");
    printf ("client: closing\n");
    exit (EXIT_SUCCESS);
}

void *server_listener_triggers (void *pargs) {
    message server_message;
    while (1) {
        if (msgrcv (my_qid, &server_message, sizeof (message), 3, 0) == -1) {
        perror ("client: msgrcv\n");
        exit (EXIT_FAILURE);
        }
        printf("server: %s", server_message.message_text.buf);
        printf(" is now online\n");
    }
}

void *server_listener_remove_event (void *pargs) {
    message server_message;
    while (1) {
        if (msgrcv (my_qid, &server_message, sizeof (message), 4, 0) == -1) {
        perror ("client: msgrcv\n");
        exit (EXIT_FAILURE);
        }
        printf("server: %s", server_message.message_text.buf);
        printf(" has been removed\n");
    }
}

void *server_listener_ask (void *pargs) {
    message server_message;
    while (1) {
        if (msgrcv (my_qid, &server_message, sizeof (message), 5, 0) == -1) {
        perror ("client: msgrcv\n");
        exit (EXIT_FAILURE);
        }
        printf("server: %s", server_message.message_text.buf);
        printf(" is now online\n");
    }
}

int main (int argc, char **argv) {

    message client_message, server_message;
    /*  Create medium for communication */
    if ((my_qid = msgget (IPC_PRIVATE, 0660)) == -1) {
        perror ("msgget: my_qid");
        exit (EXIT_FAILURE);
    }

    if ((server_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (EXIT_FAILURE);
    }

    if ((server_qid = msgget (server_queue_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (EXIT_FAILURE);
    }
    
    char *sep             = " ";
    char *sub             = "sub";
    char *ask             = "ask";
    char *list            = "list";
    char *cerrar          = "exit";
    char *unsub           = "unsub";
    char *access_denied   = "denied";
    char *token_command;

    client_message.message_type = 1;
    client_message.message_text.qid = my_qid;

    //  Establishing connection
    printf("\nclient: establishing connection with server\n");

    char buf[5];
    char buf1[12];
    sprintf (buf1, "%s", "client ");
    sprintf (buf, "%d", my_qid);
    strcat  (buf1, buf);
    strcat  (buf1, " is establishing connection");
    stpcpy  (client_message.message_text.buf, buf1);
    
    if (msgsnd (server_qid, &client_message, sizeof (message), 0) == -1) {
        perror ("client: msgsnd");
        exit (EXIT_FAILURE);
    }

    if (msgrcv (my_qid, &server_message, sizeof (message), 1, 0) == -1) {
        perror ("client: msgrcv");
        exit (EXIT_FAILURE);
    }

    /*  Verification of access approved */
    printf ("server: %s\n\n", server_message.message_text.buf);
    if (strncmp (server_message.message_text.buf, access_denied, 6) == 0) {
        printf ("server: server is full. try again later\n\n");
        exit (EXIT_SUCCESS);
    }

    pthread_t threadID_server_closed;
    pthread_t threadID_server_trigger;
    pthread_t threadID_server_remove_event;
    pthread_create (&threadID_server_closed, NULL, server_listener_closing, NULL);
    pthread_create (&threadID_server_trigger, NULL, server_listener_triggers, NULL);
    pthread_create (&threadID_server_remove_event, NULL, server_listener_remove_event, NULL);

    while (fgets (client_message.message_text.buf, SIZE_COMMAND, stdin)) {
        /*  Formating console entry */
        int length = strlen (client_message.message_text.buf);
        if (client_message.message_text.buf [length - 1] == '\n') client_message.message_text.buf [length - 1] = '\0';

        /*  Tokenization of entry   */
        char auxBuffer[SIZE_COMMAND];
        strcpy(auxBuffer, client_message.message_text.buf);
        token_command  = strtok (auxBuffer, sep);

        // Condicion de carrera?
        
        if (strcmp (token_command, sub) == 0)           client_message.message_type = 2;

        else if (strcmp (token_command, unsub) == 0)    client_message.message_type = 2;

        else if (strcmp (token_command, ask) == 0)      client_message.message_type = 3;

        else if (strcmp (token_command, list) == 0)     client_message.message_type = 4;

        else if (strcmp (token_command, cerrar) == 0) {
            exit (EXIT_SUCCESS);
            break;
        }

        else {
            printf("server: invalid command\n");
            continue;
        }

        if (msgsnd (server_qid, &client_message, sizeof (message), 0) == -1) {
            perror ("client: msgsnd");
            exit (EXIT_FAILURE);
        }

        if (msgrcv (my_qid, &server_message, sizeof (message), 1, 0) == -1) {
            perror ("client: msgrcv");
            exit (EXIT_FAILURE);
        }

        printf ("server: %s\n\n", server_message.message_text.buf);
    }

    if (msgctl (my_qid, IPC_RMID, NULL) == -1) {
        perror ("client: msgctl");
        exit (EXIT_FAILURE);
    }

    printf ("client: closed\n");

    exit (EXIT_SUCCESS);
}