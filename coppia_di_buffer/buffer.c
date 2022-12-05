#include "buffer.h"

#include <unistd.h>
#include <stdio.h>

void produzione(int sem_id, buffer *buf1, buffer *buf2, int valore)
{

    buffer* buf;

    Wait_Sem(sem_id, SPAZIO_DISP);

    if (buf1->stato == LIBERO)
    {
        printf("[%d] In produzione su buf1...\n", getpid());

        buf = buf1;

    } else {

        printf("[%d] In produzione su buf2...\n", getpid());

        buf = buf2;
    }

    buf->stato = INUSO;


    sleep(1);

    buf->valore = valore;

    buf->stato = OCCUPATO;

    printf("[%d] Prodotto il valore %d\n", getpid(), valore);


    Signal_Sem(sem_id, MESSAGGIO_DISP);
}

int consumazione(int sem_id, buffer *buf1, buffer *buf2)
{

    buffer * buf;
    int valore;


    Wait_Sem(sem_id, MESSAGGIO_DISP);

    if (buf1->stato == OCCUPATO)
    {
        printf("[%d] In consumazione su buf1...\n", getpid());

        buf = buf1;

    } else {

        printf("[%d] In consumazione su buf2...\n", getpid());

        buf = buf2;
    }

    buf->stato = INUSO;

    sleep(2);


    valore = buf->valore;

    buf->stato = LIBERO;

    printf("[%d] Consumato il valore %d\n", getpid(), valore);


    Signal_Sem(sem_id, SPAZIO_DISP);


    return valore;
}

