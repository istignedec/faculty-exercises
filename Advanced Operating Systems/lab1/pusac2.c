/* PUSAC 2, IMA DUHAN (2), TREBA PAPIR (1) I SIBICE (3) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>

struct my_msgbuf {
    long mtype;
    char mtext[200];
};

int msqid;

void retreat(int failure) {
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
    exit(0);
}

int main(void) {

    struct my_msgbuf buf;
    key_t key;

    key = 330121;

    /* spajanje na red poruka */
    if ((msqid = msgget(key, 0600 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }
    else {
        printf("\nPusac 2 spojen na red poruka.");
    }

    fflush(stdout);

    srand(time(NULL));
    sigset(SIGINT, retreat);

    while(1) {
        /* primi poruku (samo tipa 1, od pusaca 1) */
        if (msgrcv(msqid, (struct msgbuf *)&buf, sizeof(buf)-sizeof(long), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        } else {
            printf("\n(2) Primljeni sastojci : %s", buf.mtext);
            fflush(stdout);

            /* ako su primljeni 1 i 3, udji u KO, napravi nove sastojke i predaj dalje pusacu 3 */
            if(strcmp(buf.mtext, "13") == 0 || strcmp(buf.mtext, "31") == 0) {
                printf("\nPusac 2 ima sve sastojke i pusi...");
                fflush(stdout);

                sleep(5);
                int randBr1 = rand()%(3-1+1)+1;
                int randBr2;
                while((randBr2 = rand()%(3-1+1)+1) == randBr1) { }

                char text[2];
                sprintf(text, "%d%d", randBr1, randBr2);
                memcpy(buf.mtext, text, strlen(text)+1);
                buf.mtype = 3;

                if (msgsnd(msqid, (struct msgbuf *)&buf, strlen(text)+1, 0) == -1)
                    perror("msgsnd");
            }
            else { /* ako nisu 1 i 3, samo proslijedi poruku dalje pusacu 3, poruka tipa 3 */
                buf.mtype = 3;
                if (msgsnd(msqid, (struct msgbuf *)&buf, strlen(buf.mtext)+1, 0) == -1)
                    perror("msgsnd");
            }
        }
    }
    
    return 0;
}