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

typedef struct message_text {
    int qid;
    char buf [SIZE_COMMAND]; //Toque esto, era 200
} message_text;

struct message {
    long message_type;
    message_text message_text;
};

key_t msg_queue_key;
int qid;
struct message message;

void *client_listener (void *parg) {
    while (1) {

        if (msgrcv (qid, &message, sizeof (struct message_text), 1, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }

        printf ("Server: message received.\n");


        message.message_text.buf[0] = "Pito corto";

        int client_qid = message.message_text.qid;
        message.message_text.qid = qid;

        if (msgsnd (client_qid, &message, sizeof (struct message_text), 0) == -1) {  
            perror ("msgget");
            exit (EXIT_FAILURE);
        }

        printf ("Server: response sent to client.\n");
    }
}

int main (int argc, char **argv)
{
    pthread_t threadID_client_listener;

    pthread_create(&threadID_client_listener, NULL, client_listener, NULL);

    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (EXIT_FAILURE);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (EXIT_FAILURE);
    }

    printf ("Server: Hello, World!\n");

    while (1) {
        
    }
    
}