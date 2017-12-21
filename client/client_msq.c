#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include "msq_header.h"
static int ende = 1;
static int setup_client (key_t key, int flag) {
   int res;
   res = msgget (key, flag);
   if (res == -1) {
      printf ("Client-Fehler beim Einrichten"
              " der Message Queues...\n");
      return -1;
   }
   return res;
}
/* Der Client will sich beenden */
static void shutdown_msq (int signr) {
      ende = 0;
}
int main (void) {
   int server_id, client_id;
   int res;
   client2server c2s;
   server2client s2c;
   /* Signalhandler für STRG+C einrichten */
   signal (SIGINT, shutdown_msq);
   /* Eine Message Queue zum Server */
   server_id = setup_client (KEY, 0);
   if (server_id < 0)
      return EXIT_FAILURE;
   /* Eine Message Queue für den Client */
   client_id = setup_client (IPC_PRIVATE, PERM | IPC_CREAT);
   if (client_id < 0)
      return EXIT_FAILURE;
   /* Eine Nachricht an den Server versenden */
   c2s.prioritaet = 2;
   sprintf (c2s.message, "%d:Login", client_id);
   res = msgsnd (server_id, &c2s, MSG_LEN, 0);
   if (res == -1) {
      printf ("Konnte keine Nachricht versenden ...\n");
      return EXIT_FAILURE;
   }
   /* Bestätigung des Servers oder Rundschreiben */
   /* von anderen Clients empfangen              */
   res = msgrcv (client_id, &s2c, MSG_LEN, 0, 0);
   if (res == -1) {
      printf ("Fehler beim Erhalt der Nachricht ...\n");
      return EXIT_FAILURE;
   }
   /*Bestätigung oder Rundschreiben auslesen und ausgeben*/
   printf ("%ld: %s\n", s2c.prioritaet, s2c.message);
   while (ende) {
  /* Hier könnte der wichtige Code zur Kommunikation  */
  /* zwischen  den Prozessen geschrieben werden.      */
  /* In diesem Beispiel werden nur die neu erstellten */
  /* Message Queues als neue User ausgegeben. Die     */
  /* Schleife wartet auf das Signal SIGINT == STRG+C  */
      /* Bestätigung oder Rundschreiben empfangen */
      res= msgrcv (client_id, &s2c, MSG_LEN, 0, IPC_NOWAIT);
      if (res != -1) {
         printf ("(%s) von User mit der Message-Queue-ID: "
                 " %ld\n", s2c.message, s2c.prioritaet);
      }
      /* Dauerndes Pollen belastet unnötig die CPU      */
      /* – Eine Bremse (siehe top mit und ohne usleep() */
      usleep( 1000 );
   }
   /* STRG+C also das Signal SIGINT wurde ausgelöst ... */
   c2s.prioritaet = 1;
   sprintf (c2s.message, "%d", client_id);
   res = msgsnd (server_id, &c2s, MSG_LEN, 0);
   if (res == -1) {
      printf ("Konnte keine Nachricht versenden ...\n");
      return EXIT_FAILURE;
   }
   /* Message Queues entfernen */
   msgctl (client_id, IPC_RMID, NULL);
   return EXIT_SUCCESS;
}
