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

int main (int argc, char **argv) {

    message client_message, server_message;

    // create my client queue for receiving messages from server
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

    //char no[2]              = "n"; // Later view how to implement this
    //char yes[2]             = "y";
    char sep[2]             = " ";
    char sub[4]             = "sub";
    char ask[4]             = "ask";
    char list[5]            = "list";
    char unsub[6]           = "unsub";
    char cerrar[6]          = "exit";
    char access_denied[7]   = "denied";
    char *token_command;

    client_message.message_type = 1;
    client_message.message_text.qid = my_qid;

    //  Establishing connection
    printf("\nclient: establishing connection with server\n");

    char buf[5];
    char buf1[12];
    sprintf (buf1, "%s", "Client ");
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
    pthread_create (&threadID_server_closed, NULL, server_listener_closing, NULL);


    printf ("client: please type in a command: ");

    while (fgets (client_message.message_text.buf, SIZE_COMMAND, stdin)) {
        /*  Formating console entry */
        int length = strlen (client_message.message_text.buf);
        if (client_message.message_text.buf [length - 1] == '\n') client_message.message_text.buf [length - 1] = '\0';

        /*  Tokenization of entry   */
        token_command  = strtok (client_message.message_text.buf, sep);

        // Condicion de carrera
        printf("token: %s\n", token_command);
        for (int i = 0; i < strlen(token_command); i++) printf("%d ", token_command[i]);
        printf("\n");
        for (int i = 0; i < strlen(cerrar); i++) printf("%d ", cerrar[i]);
        //printf("token value no n: %d\n", strncmp(token_command, cerrar, 4));
        //printf("token value yes n: %d\n", strcmp(token_command, cerrar));

        if (strncmp(token_command, sub, 3) == 0) client_message.message_type = 2;

        if (strncmp(token_command, unsub, 5) == 0) client_message.message_type = 2;

        if (strncmp(token_command, ask, 3) == 0)    client_message.message_type = 3;

        if (strncmp(token_command, list, 4) == 0)   client_message.message_type = 4;

        if (strncmp(token_command, cerrar, 4) == 0) {
            exit (EXIT_SUCCESS);
            break;
            
            /*printf ("server: are you sure? type (y) or (n): ");
            fgets (buf, 2, stdin);
            length = strlen(buf);
            if (buf[length - 1] == '\n') buf [length - 1] = '\0';
            if (strcmp(buf, yes) ==  0) break;
            if (strcmp(buf, no) == 0) continue;
            else {
                printf("server: invalid command\n");
                continue;
            }*/ //  Segmention Fault
        }

        else {
            printf("server: invalid command\n");
            printf ("client: please type in a command: ");
            continue;
        }
        /*  End of my shit    */

        if (msgsnd (server_qid, &client_message, sizeof (message), 0) == -1) {
            perror ("client: msgsnd");
            exit (EXIT_FAILURE);
        }

        if (msgrcv (my_qid, &server_message, sizeof (message), 1, 0) == -1) {
            perror ("client: msgrcv");
            exit (EXIT_FAILURE);
        }

        printf ("server: %s\n\n", server_message.message_text.buf);  

        printf ("client: please type a command: ");
    }

    if (msgctl (my_qid, IPC_RMID, NULL) == -1) {
        perror ("client: msgctl");
        exit (EXIT_FAILURE);
    }

    printf ("client: closed\n");

    exit (EXIT_SUCCESS);
}