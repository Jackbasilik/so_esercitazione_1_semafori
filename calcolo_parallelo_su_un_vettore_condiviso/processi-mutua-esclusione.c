#include "processi.h"
#include "semafori.h"

#include <sys/wait.h>
#include <limits.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

int inizializza_semafori()
{
    key_t chiave_sem = IPC_PRIVATE;

    int sem_id = semget(chiave_sem, 1, IPC_CREAT | 0644); 

    if (sem_id < 0)
    {
        perror("Impossibile creare il semaforo per la mutua esclusione");
        exit(1);
    }

    /* Valore iniziale: 1 (mutua esclusione) */
     semctl(sem_id, 0, SETVAL, 1);

    /* TBD: inizializzare il mutex */
    

    return sem_id;
}

void figlio(int *vettore,
            int *buffer,
            int sem_id,
            int elemento_iniziale,
            int qta_elementi)
{
    
    int minimo = INT_MAX;

    for (int i = elemento_iniziale; i < elemento_iniziale + qta_elementi; i++)
    {
        if (vettore[i] < minimo){
         minimo = vettore[i];
        }
    }

    printf("Figlio: Il minimo locale è %d\n", minimo);

    Wait_Sem(sem_id, 0);
    
    if (minimo < *buffer){
        *buffer = minimo;
    }
    
    Signal_Sem(sem_id, 0);
}


void padre(int *buffer,
           int sem_id)
{

    /* Attesa terminazione processi figli */
    wait(NULL);

    /* Risultato finale */
    printf("Padre: Il valore minimo assoluto è: %d\n", *buffer);
}
