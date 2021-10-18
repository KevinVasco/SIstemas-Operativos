#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "./server_key_path.txt"
#define PROJECT_ID 'M'
#define SIZE_COMMAND 256

typedef struct message_text {
    int qid;
    char buf [SIZE_COMMAND]; // I touched this, the value was 200
}message_text;

typedef struct message {
    long message_type;
    message_text message_text;
}message;

int main (int argc, char **argv)
{
    /*  Communicaction shit */
    key_t server_queue_key;
    int server_qid, myqid;
    message client_message, server_message;

    // create my client queue for receiving messages from server
    if ((myqid = msgget (IPC_PRIVATE, 0660)) == -1) {
        perror ("msgget: myqid");
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
    /*  End of communication shit   */

    char *token;
    char sep[2]     = " ";
    char sub[4]     = "sub";
    char ask[4]     = "ask";
    char list[5]    = "list";
    char unsub[6]   = "unsub";
    char close[6]   = "exit";

    client_message.message_type = 1;
    client_message.message_text.qid = myqid;

    //  Establishing connection
    printf("Establishing connection with server\n");

    char buf[5];
    char buf1[12];
    sprintf(buf1, "%s", "Client ");
    sprintf(buf, "%d", myqid);
    strcat(buf1, buf);
    strcat(buf1, " is establishing connection");
    stpcpy(client_message.message_text.buf, buf1);
    
    if (msgsnd (server_qid, &client_message, sizeof (message), 0) == -1) {
        perror ("client: msgsnd");
        exit (EXIT_FAILURE);
    }

    if (msgrcv (myqid, &server_message, sizeof (message), 1, 0) == -1) {
        perror ("client: msgrcv");
        exit (EXIT_FAILURE);
    }

    printf ("server: %s\n\n", server_message.message_text.buf);  


    printf ("Please type in a command: ");

    while (fgets (client_message.message_text.buf, SIZE_COMMAND, stdin)) {

        int length = strlen (client_message.message_text.buf);
        if (client_message.message_text.buf [length - 1] == '\n') client_message.message_text.buf [length - 1] = '\0';

        /*  Start of my shit    */
        token  = strtok (client_message.message_text.buf, sep);

        if (strcmp(token, sub) == 0)    client_message.message_type = 2;

        if (strcmp(token, ask) == 0)    client_message.message_type = 3;

        if (strcmp(token, list) == 0)   client_message.message_type = 4;

        if (strcmp(token, unsub) == 0)  client_message.message_type = 5;

        if (strcmp(token, close) == 0) {
            /*
            * 1. List all that i have and unsub to all?
            * 2. break out of while instead of leaving it here
            */
            break;
        }
        /*  End of my shit    */

        if (msgsnd (server_qid, &client_message, sizeof (message), 0) == -1) {
            perror ("client: msgsnd");
            exit (EXIT_FAILURE);
        }

        if (msgrcv (myqid, &server_message, sizeof (message), 0, 0) == -1) {
            perror ("client: msgrcv");
            exit (EXIT_FAILURE);
        }

        printf ("Message received from server: %s\n\n", server_message.message_text.buf);  

        printf ("Please type a command: ");
    }

    if (msgctl (myqid, IPC_RMID, NULL) == -1) {
        perror ("client: msgctl");
        exit (EXIT_FAILURE);
    }

    printf ("client: closed\n");

    exit (EXIT_SUCCESS);
}