#include "init.h"
#include "syscall.h"

int counter_init;

void processo_b(void)
{
    so_escreve('B');
    so_escreve(10);
    while(1){}
}


//primeiro programa: fica em loop (apenas para teste)
void init(void)
{
    int pid_b;

    pid_b = so_cria_processo((int) processo_b);
    if (pid_b == -1)
    {
        so_escreve('E');
    }
    else
    {
        so_escreve('C');
    }

    so_escreve(10);

    so_escreve('O');
    so_escreve('i');
    so_escreve(10);
    
    counter_init = 0;
    while (1)
    {
        counter_init++;
        if (counter_init == 100)
        {
            counter_init = 0;
        }
    }
}
