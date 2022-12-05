#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"

int main()
{

    int buf1_id;
    int buf2_id;
    buffer *buf1;
    buffer *buf2;
    int sem_id;

    //acquisizione buffer1
    key_t chiave_buf1 = ftok(".", 'a');
    buf1_id = semget(chiave_buf1, sizeof(buffer), 0);

    if (buf1_id < 0)
    {
        perror("Errore accesso SHM buf1");
        exit(1);
    }

    //acquisizione buffer2
    key_t chiave_buf2 = ftok(".", 'b');
    buf2_id = semget(chiave_buf2, sizeof(buffer), 0);

    if (buf2_id < 0)
    {
        perror("Errore accesso SHM buf2");
        exit(1);
    }

    //acquisizione puntarore a buffer1
    buf1 = (buffer*) shmat(chiave_buf1, NULL, 0);

    if (buf1 == (void *)-1)
    {
        perror("Errore attach SHM buf1(main-consumatore)");
        exit(1);
    }

    //acquisizione puntarore a buffer2
    buf2 = (buffer*) shmat(chiave_buf2, NULL, 0);

    if (buf2 == (void *)-1)
    {
        perror("Errore attach SHM buf2");
        exit(1);
    }

    //acquisizione semafori
    key_t chiave_sem = ftok(".", 'c');
    sem_id = semget(chiave_sem, 2, 0);

    if (sem_id < 0)
    {
        perror("Errore accesso semafori");
        exit(1);
    }



    for (int i = 0; i < 5; i++)
    {
        int valore = consumazione(sem_id, buf1, buf2);
    }

    exit(0);
}
