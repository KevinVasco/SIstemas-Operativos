#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <pthread.h>

#define SERVER_KEY_PATHNAME "./server_key_path.txt"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660
#define SIZE_COMMAND 256
//#define SIZE_REGISTERS 128                              //  Maximum amount of clients able to conenct to the server at any given point

typedef struct message_text {
    int qid;
    char buf [SIZE_COMMAND];
} message_text;

typedef struct message {
    long message_type;
    message_text message_text;
}message;

key_t msg_queue_key;
int qid;

//  Organize
void *client_first_connection (void *parg) {

    message message_client, message_server;

    while (1) {

        // read an incoming message
        if (msgrcv (qid, &message_client, sizeof (message), 1, 0) == -1) {
            perror ("msgrcv");
            exit (1);
        }

        printf ("Server: %s\n", message_client.message_text.buf);

        char buf[20];
        sprintf(buf, "%s", "Access allowed");
        sprintf(message_server.message_text.buf, "%s", "Access allowed");
        //strcat(message_server.message_text.buf, "Access allowed");

        int client_qid = message_client.message_text.qid;
        message_client.message_text.qid = qid;

        message_server.message_text.qid = qid;
        message_server.message_type = 1;

        // send reply message to client
        if (msgsnd (client_qid, &message_server, sizeof (message), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }

        printf ("Server: response sent to client.\n");
    }
}

void *client_listener_sub (void *parg) {
    message message_client;
    //message message_server;

    while (1) {

        if (msgrcv (qid, &message_client, sizeof (message), 2, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }

        int client_qid = message_client.message_text.qid;

        printf ("Server: message received from client %d.\n", client_qid);

        // process message
        int length = strlen (message_client.message_text.buf);
        char buf [20];
        sprintf (buf, " %d", length);
        strcat (message_client.message_text.buf, buf);

        message_client.message_text.qid = qid;
        //printf("El 1\n");

        // send reply message to client
        if (msgsnd (client_qid, &message_client, sizeof (message), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }

        printf ("Server: response sent to client %d.\n", client_qid);
    }
}

int main (int argc, char **argv)
{
    //Create the server_key_path.txt so that i don't have to give it for him

    pthread_t threadID_connection;
    pthread_t threadID_client_listener_sub;
    //pthread_t threadID_client_listener_ask;
    //pthread_t threadID_client_listener_list;
    //pthread_t threadID_client_listener_unsub;

    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (EXIT_FAILURE);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (EXIT_FAILURE);
    }

    pthread_create (&threadID_connection, NULL, client_first_connection, NULL);
    pthread_create (&threadID_client_listener_sub, NULL, client_listener_sub, NULL);
    //pthread_create(&threadID_client_listener_ask, NULL, client_listener_ask, &register_clients);
    //pthread_create(&threadID_client_listener_list, NULL, client_listener_list, &register_clients);
    //pthread_create(&threadID_client_listener_unsub, NULL, client_listener_unsub, &register_clients);


    printf ("Server: Hello, World!\n");

    char terminal_input[SIZE_COMMAND];
    char *token;
    char sep[2] = " ";
    char close[5] = "exit";

    while (fgets (terminal_input, SIZE_COMMAND, stdin)) {
        //Add confirmation
        int length = strlen (terminal_input);
        if (terminal_input [length - 1] == '\n') terminal_input [length - 1] = '\0';

        token = strtok(terminal_input, sep);
        if (strcmp (token, close) == 0) {
            printf ("server: closing\n");
            if (msgctl (qid, IPC_RMID, NULL) == -1) {
                perror ("server: msgctl");
                exit (EXIT_FAILURE);
            }
        }
    }
    printf ("server: something went wrong");
    exit (EXIT_FAILURE);
}