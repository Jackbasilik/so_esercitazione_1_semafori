#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>

#include "processi.h"
#define NUM_ELEMENTI 10000 
#define NUM_PROCESS 10
#define ELEM_PER_PROC 1000

//il programma ha un bug con l'inizializzazione del vettore la seconda volta che si avvia il proramma

int main()
{

    int vett_id;
    int buffer_id;
    int sem_id;
    int *vettore;
    int *buffer;

    key_t chiave_vet = IPC_PRIVATE;
    vett_id = shmget(chiave_vet, NUM_ELEMENTI * sizeof(int), IPC_CREAT | 0644); 

     if (vett_id < 0){
        perror("Impossibile creare l'array di interi condiviso");
        exit(1);
    }

    vettore = (int*)shmat(chiave_vet, NULL, 0);

    if (vettore == (void *)-1)
    {
        perror("Impossibile inizializzare l'array di interi condiviso");
        exit(1);
    }

    /* Inizializza il vettore con numeri casuali tra 0 e INT_MAX */
    srand(12345);

    for (int i = 0; i < NUM_ELEMENTI; i++)
    {
        vettore[i] = rand() % INT_MAX;
        printf("%d\n", vettore[i]); // per debugging
    }

    key_t chiave_buf = IPC_PRIVATE;
    buffer_id = shmget(chiave_buf, sizeof(int), IPC_CREAT | 0644);

    if (buffer_id < 0)
    {
        perror("Impossibile creare il buffer condiviso");
        exit(1);
    }

    buffer = (int*) shmat(chiave_buf, NULL, 0);

    if (buffer == (void *)-1)
    {
        perror("Impossibile inizializzare il buffer condiviso");
        exit(1);
    }

    /* Inizializza il buffer ad INT_MAX.
     * Il valore da ricercare sarà, per definizione, minore del valore iniziale.
     */
    *buffer = INT_MAX;


    /* Inizializzazione semafori */
    sem_id = inizializza_semafori();

    /* Avvio dei processi figli */

    pid_t pid[NUM_PROCESS];

    for(int i=0; i<NUM_PROCESS; i++)
    {
        pid[i] = fork();
        if(pid[i] == 0){
            int elemento_iniz = i * ELEM_PER_PROC;
            figlio(vettore, buffer, sem_id, elemento_iniz, ELEM_PER_PROC);
            exit(0);
         }
         else if (pid[i] < 0){
            perror("ERRORE FORK");
            exit(1);
         }
         else {padre(buffer, sem_id);}
            
    }

    /* Deallocazione risorse IPC */
    semctl(sem_id, 0, IPC_RMID);
    shmctl(vett_id, IPC_RMID, 0);
    shmctl(buffer_id, IPC_RMID, 0);
}
