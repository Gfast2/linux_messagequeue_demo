#include <stdlib.h>
#include <signal.h>
/* Magische Nummer */
#define KEY 1234L
/* Begrenzung der Nachricht */
#define MSG_LEN 1024
/* Zugriffsrechte */
#define PERM 0666
#undef signal
/* Datentypen zum Senden und Empfangen der Nachrichten */
typedef struct {
    long prioritaet;
    char message[MSG_LEN];
} client2server;
typedef struct {
    long prioritaet;
    char message[MSG_LEN];
} server2client;
struct id_verwaltung {
    int id;
    struct id_verwaltung *next;
};

struct id_verwaltung *id_first = NULL;

static void mq_verwaltung_add( int i ) {
    struct id_verwaltung *ptr;
    if( id_first == NULL) {
        id_first = malloc(sizeof( struct id_verwaltung ));
        if( id_first == NULL )
            exit(EXIT_FAILURE);
        id_first->id = i;
        id_first->next = NULL;
    }
    else {
        ptr = id_first;
        while(ptr->next != NULL)
            ptr=ptr->next;
        ptr->next = malloc(sizeof( struct id_verwaltung ));
        ptr = ptr->next;
        ptr->id = i;
        ptr->next = NULL;
    }
}   
static void mq_verwaltung_remove( int i ) {
    struct id_verwaltung *ptr_tmp;
    struct id_verwaltung *ptr;
    if(id_first == NULL)
        return;
    if( id_first->id == i ) {
        ptr = id_first->next;
        free(id_first);
        id_first = ptr;
        printf("User %d hat sich ausgeloggt\n",i);
        return;
    }
    ptr = id_first;
    while(ptr->next != NULL) {
        ptr_tmp = ptr->next;
        if( ptr_tmp->id == i ) {
            ptr->next = ptr_tmp->next;
            free(ptr_tmp);
            printf("User %d hat sich ausgeloggt\n",i);
            break;
        }
        ptr=ptr_tmp;
    }
}
typedef void (sigfunk) (int);
sigfunk * signal (int sig_nr, sigfunk signalhandler) {
    struct sigaction neu_sig, alt_sig;
    neu_sig.sa_handler = signalhandler;
    sigemptyset (&neu_sig.sa_mask);
    neu_sig.sa_flags = SA_RESTART;
    if (sigaction (sig_nr, &neu_sig, &alt_sig) < 0)
        return SIG_ERR;
    return alt_sig.sa_handler;
}
