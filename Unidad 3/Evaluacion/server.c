#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define SERVER_KEY_PATHNAME "./server_key_path.txt"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660                          //  Read and write
#define SIZE_COMMAND 256                                //  Maximum amount chars for command
#define SIZE_REGISTERS 128                              //  Maximum amount of clients able to conenct to the server at any given point
#define SIZE_TOTAL_EVENTS 128                           //  Maximum amount of events
#define SIZE_EVENTS_NAME 64                             //  Maximum amount of chars for a name_event

typedef struct message_text {
    int qid;
    char buf [SIZE_COMMAND];
} message_text;

typedef struct message {
    long message_type;
    message_text message_text;
}message;

typedef struct event {
    char name_event[SIZE_EVENTS_NAME];
    int interested;
    int *attendents;
    int max_capacity;
}event;

key_t msg_queue_key;
struct event *events;
int qid;
int number_events;
int number_clients;
int register_clientes[SIZE_REGISTERS];

void *client_first_connection (void *parg) {
    message message_client, message_server;
    /*  Init register clients   */
    number_clients = 0;
    while (1) {
        /*  Read incoming message thru channel 1    */
        if (msgrcv (qid, &message_client, sizeof (message), 1, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        printf ("server: %s\n", message_client.message_text.buf);

        /*  Preparing response  */
        int client_qid = message_client.message_text.qid;
        message_server.message_text.qid = qid;
        message_server.message_type = 1;
        
        if (number_clients == SIZE_REGISTERS - 1) sprintf(message_server.message_text.buf, "%s", "denied access");
        else {
            sprintf(message_server.message_text.buf, "%s", "granted access");
            /*  Adding new client to registers  */
            register_clientes [number_clients] = message_client.message_text.qid;
            number_clients ++;
            printf ("server: added new client.\n");
        }
        /*  Send status to client */
        if (msgsnd (client_qid, &message_server, sizeof (message), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }
    }
}

void *client_listener_sub_unsub (void *parg) {
    message message_client, message_server;
    char *token;
    char *sep = " ";
    char *sub = "sub";
    char *unsub = "unsub";
    /*  Read incoming message thru channel 2    */
    while (1) {
        if (msgrcv (qid, &message_client, sizeof (message), 2, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        int client_qid = message_client.message_text.qid;

        /*  Preparing response  */
        token = strtok(message_client.message_text.buf, sep);
        /*  Sub command */
        if (strcmp (token, sub) == 0) {
            /*  Default message */
            sprintf(message_server.message_text.buf, "%s", "event not found");
            token = strtok(NULL, sep);
            /*  No events for subscription   */
            if (number_events == 0) sprintf(message_server.message_text.buf, "%s", "event not found");
            for (int i = 0; i < number_events; i++) {
                if (strcmp (token, events[i].name_event) == 0) {
                    /*  Event is full   */
                    if (events[i].interested == events[i].max_capacity) {
                        sprintf(message_server.message_text.buf, "%s", "event is full");
                        break;
                    }
                    /*  Event not full  */
                    events[i].attendents[events[i].interested] = client_qid;
                    events[i].interested ++;
                    sprintf(message_server.message_text.buf, "%s", "you are now subbed");
                }
            }
        }
        /*  Unsub command   */
        else if (strcmp (token, unsub) == 0) {
            token = strtok(NULL, sep);
            int pos = -1;
            int posEvent = 0;
            sprintf(message_server.message_text.buf, "%s", "you are not subbed to this event");
            for (int i = 0; i < number_events; i++) {
                if (strcmp (token, events[i].name_event) == 0) {
                    for (int j = 0; j < events[i].interested; j++) {
                        if (client_qid == events[i].attendents[j]) {
                            pos = j;
                            posEvent = i;
                            break;
                        }
                    }
                    if (pos == 0) {
                        events[i].attendents[0] = 0;
                    } else {
                        for (int j = pos; j < events[i].interested; j++) {
                            events[i].attendents[j] = events[i].attendents[j + 1];
                        }
                    }
                }
            }
            events[posEvent].interested --;
        }

        message_server.message_text.qid = qid;
        message_server.message_type = 1;

        /*  Send reply message to client    */
        if (msgsnd (client_qid, &message_server, sizeof (message), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }

        printf ("server: response sent to client %d.\n", client_qid);
    }
}

void *client_listener_ask (void *pargs) {
    message message_client, message_server;
    /*  Read incoming message thru channel 3    */
    while(1) {
        if (msgrcv (qid, &message_client, sizeof (message), 3, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        int client_qid = message_client.message_text.qid;
        message_server.message_text.qid = qid;
        message_server.message_type = 5;
        for (int i = 0; i < number_events; i++) {
            if (events[i].interested < events[i].max_capacity) {
                sprintf(message_server.message_text.buf, "%s", events[i].name_event);
                if (msgsnd (client_qid, &message_server, sizeof (message), 0) == -1) {  
                    perror ("msgget");
                    exit (1);
                }
            }
        }
        printf ("server: response sent to client %d.\n", client_qid);
    }
}

void *client_listener_list (void *pargs) {
    message message_client, message_server;
    /*  Read incoming message thru channel 4    */
    while(1) {
        if (msgrcv (qid, &message_client, sizeof (message), 4, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        int client_qid = message_client.message_text.qid;
        message_server.message_text.qid = qid;

        message_server.message_type = 6;
        for (int i = 0; i < number_events; i++) {
            for (int j = 0; j < events[i].interested; j ++) {
                if (client_qid == events[i].attendents[j]) {
                    sprintf(message_server.message_text.buf, "%s", events[i].name_event);
                    if (msgsnd (client_qid, &message_server, sizeof (message), 0) == -1) {  
                        perror ("msgget");
                        exit (1);
                    }
                    break;
                }
            }
        }
    printf ("server: response sent to client %d.\n", client_qid);
    }
}

int main (int argc, char **argv) {
    message server_message;
    /*  Creation of a file for communication of server key if it doesn't exist  */
    FILE *server_key_path_create = fopen(SERVER_KEY_PATHNAME,"w+");
    if (server_key_path_create == NULL){
        perror("open file fails: ");
        return(EXIT_FAILURE);
    }

    pthread_t threadID_connection;
    pthread_t threadID_client_listener_ask;
    pthread_t threadID_client_listener_list;
    pthread_t threadID_client_listener_sub_unsub;

    /*  Creation of unique key  */
    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (EXIT_FAILURE);
    }
    /*  Obtaining queue */
    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (EXIT_FAILURE);
    }

    /*  Creation of threads incharge of listing thru various channels   */
    pthread_create (&threadID_connection, NULL, client_first_connection, NULL);
    pthread_create (&threadID_client_listener_ask, NULL, client_listener_ask, NULL);
    pthread_create (&threadID_client_listener_list, NULL, client_listener_list, NULL);
    pthread_create (&threadID_client_listener_sub_unsub, NULL, client_listener_sub_unsub, NULL);

    /*  Init Events */
    number_events = 0;
    events = malloc(sizeof(event) * SIZE_TOTAL_EVENTS);

    printf ("server: hello, world\n");

    char *token;
    char *sep     = " ";
    char *add     = "add";
    char *list    = "list";
    char *close   = "exit";
    char *remove  = "remove";
    char *trigger = "trigger";
    char terminal_input[SIZE_COMMAND];

    while (fgets (terminal_input, SIZE_COMMAND, stdin)) {
        /*  Formating console entry */
        int length = strlen (terminal_input);
        if (terminal_input [length - 1] == '\n') terminal_input [length - 1] = '\0';
        /*  Tokenization of entry   */
        token = strtok (terminal_input, sep);

        /*  Add command */
        if (strcmp (token, add) == 0) {
            token = strtok(NULL, sep);
            events[number_events].interested = 0;
            events[number_events].max_capacity = 64;                //  Default 64. If necessary, do more strtok for value. Example command: add event_name max_capacity
            events[number_events].attendents = malloc (sizeof(int) * events[number_events].max_capacity);
            strcpy (events[number_events].name_event, token);
            number_events ++;
            printf("server: event added\n");
        }

        /*  Remove Command  */
        //  Add notify clients when remove
        if (strcmp (token, remove) == 0) {
            int pos = 0;
            token = strtok (NULL, sep);
            for (int i = 0; i < number_events; i++) {
                if (strcmp (token, events[i].name_event) == 0) {
                    server_message.message_type = 4;
                    sprintf(server_message.message_text.buf, "%s",events[i].name_event);
                    for (int j = 0; j < events[i].interested; j++) {
                        if (msgsnd (events[i].attendents[j], &server_message, sizeof (message), 0) == -1) {
                            perror ("msgget");
                        }
                    }
                    free(events[i].attendents);
                    pos = i;
                    break;
                }
            }
            for (int i = pos; i < number_events; i++) {
                events[i] = events[i + 1];
                if (i == number_events - 1) {
                    free (events[i].attendents);
                }
            }
            number_events --;
            printf("server: event removed\n");
        }

        /*  Trigger Command */
        if (strcmp (token, trigger) == 0) {
            token = strtok (NULL, sep);
            // Verify if token is null -> no event_name passed
            for (int i = 0; i < number_events; i++) {
                if (strcmp (token, events[i].name_event) == 0) {
                    server_message.message_type = 3;
                    server_message.message_text.qid = qid;
                    for (int j = 0; j < events[i].interested; j++) {
                        sprintf(server_message.message_text.buf, "%s", events[i].name_event);
                        if (msgsnd (events[i].attendents[j], &server_message, sizeof (message), 0) == -1) {
                            perror ("msgget");
                        }
                        printf("server: response sent to client %d\n", events[i].attendents[j]);
                    }
                    break;
                }
            }
        }

        /*  List clients subscribed to event_name   */
        if (strcmp (token, list) == 0) {
            printf("server: if no clients are shown, the event does not exist or it has no subscribers\n");
            token = strtok( NULL, sep);
            for (int i = 0; i < number_events; i++) {
                if (strcmp (token, events[i].name_event) == 0) {
                    for (int j = 0; j < events[i].interested; j++) {
                        printf("server: %s: %d\n", events[i].name_event, events[i].attendents[j]);
                    }
                break;
                }
            }
            printf("server: end of list\n");
        }

        /*  Exit Command    */
        if (strcmp (token, close) == 0) {
            printf ("server: closing\n");
            server_message.message_type = 2;
            server_message.message_text.qid = qid;
            for (int i = 0; i < number_events; i++) { free(events[i].attendents); }
            for (int i = 0; i < number_clients; i++) {
                sprintf(server_message.message_text.buf, "closing server");
                if (msgsnd (register_clientes[i], &server_message, sizeof (message), 0) == -1) {  
                    perror ("msgget");
                    exit (EXIT_FAILURE);
                }
            }
            free(events);
            /*  Closing queue   */
            if (msgctl (qid, IPC_RMID, NULL) == -1) {
                perror ("server: msgctl");
                exit (EXIT_FAILURE);
            }
        }
    }

    /*  While fails for some reason */
    for (int i = 0; i < number_events; i++) { free(events[i].attendents); }
    free(events);
    printf ("server: something went wrong");
    exit (EXIT_FAILURE);
}