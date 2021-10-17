#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "./server_key_path.txt"
#define PROJECT_ID 'M'
#define SIZE_COMMAND 256

struct message_text {
    int qid;
    char buf [SIZE_COMMAND]; // I touched this, the value was 200
};

struct message {
    long message_type;
    struct message_text message_text;
};

int main (int argc, char **argv)
{
    /*  Communicaction shit */
    key_t server_queue_key;
    int server_qid, myqid;
    struct message my_message, return_message;

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
    char close[6]   = "close";

    // I changed this with ifs so i can set the type of id
    //my_message.message_type = 1;
    my_message.message_text.qid = myqid;

    printf ("Please type in a command: ");

    while (fgets (my_message.message_text.buf, SIZE_COMMAND, stdin)) {

        int length = strlen (my_message.message_text.buf);
        if (my_message.message_text.buf [length - 1] == '\n') my_message.message_text.buf [length - 1] = '\0';

        /*  Start of my shit    */
        token  = strtok (my_message.message_text.buf, sep);

        if (strcmp(token, sub) == 0)    my_message.message_type = 1;

        if (strcmp(token, ask) == 0)    my_message.message_type = 2;

        if (strcmp(token, list) == 0)   my_message.message_type = 3;

        if (strcmp(token, unsub) == 0)  my_message.message_type = 4;

        if (strcmp(token, close) == 0) {
            /*
            * 1. List all that i have and unsub to all?
            * 2. break out of while instead of leaving it here
            */
            break;
        }
        /*  End of my shit    */

        if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
            perror ("client: msgsnd");
            exit (EXIT_FAILURE);
        }

        if (msgrcv (myqid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("client: msgrcv");
            exit (EXIT_FAILURE);
        }

        printf ("Message received from server: %s\n\n", return_message.message_text.buf);  

        printf ("Please type a command: ");
    }
    // See if this matters in some way
    if (msgctl (myqid, IPC_RMID, NULL) == -1) {
        perror ("client: msgctl");
        exit (EXIT_FAILURE);
    }

    printf ("Client: bye\n");

    exit (EXIT_SUCCESS);
}