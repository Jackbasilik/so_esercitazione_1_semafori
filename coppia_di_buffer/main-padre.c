#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "buffer.h"

int main()
{

    int buf1_id;
    int buf2_id;
    buffer *buf1;
    buffer *buf2;
    int sem_id;

    //creazione buffer1
    key_t chiave_buf1 = ftok(".", 'a');
    buf1_id = shmget(chiave_buf1, sizeof(buffer), IPC_CREAT | 0644);

    if (buf1_id < 0)
    {
        perror("Errore creazione SHM buf1");
        exit(1);
    }

    //creazione buffer2
    key_t chiave_buf2 = ftok(".", 'b');
    buf2_id = shmget(chiave_buf2, sizeof(buffer), IPC_CREAT | 0644);

    if (buf2_id < 0)
    {
        perror("Errore creazione SHM buf2");
        exit(1);
    }

    //acquisizione puntatore buffer1
    buf1 = (buffer*) shmat(chiave_buf1, NULL, 0);

    if (buf1 == (void *)-1)
    {
        perror("Errore attach SHM buf1");
        exit(1);
    }

    //acquisizione puntatore buffer2
    buf2 = (buffer*) shmat(chiave_buf2, NULL, 0);

    if (buf2 == (void *)-1)
    {
        perror("Errore attach SHM buf2");
        exit(1);
    }

    //inzializzazione buffer
    buf1->stato = LIBERO;
    buf2->stato = LIBERO;

    //creazione semafori
    key_t chiave_sem = ftok(".", 'c');
    sem_id = semget(chiave_sem, 2, IPC_CREAT | 0644);

    if (sem_id < 0)
    {
        perror("Errore creazione semafori");
        exit(1);
    }

    semctl(sem_id, SPAZIO_DISP, SETVAL, 2);
    semctl(sem_id, MESSAGGIO_DISP, SETVAL, 0);

    //creazione processo figlio produttore
    pid_t pid1 = fork();

    if(pid1 == 0){
        //il primo valore è il percorso, il secondo invece il nome dell'eseguibile, null è la fine della lista 
        execl("./main-produttore", "main-produttore", NULL); 
    }
    else{
        perror("ERRORE FORK PRODUTTORE");
        exit(1);
    }

    //creazione processo figlio consumatore
    pid_t pid2 = fork();

    if(pid2 == 0){
        execl("./main-consumatore", "main-consumatore", NULL);
    }
    else{
        perror("ERRORE FORK CONSUMATORE");
        exit(1);
    }


    for (int i = 0; i < 2; i++)
    {
        wait(NULL);
    }

    shmctl(buf1_id, IPC_RMID, NULL);
    shmctl(buf2_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
}
