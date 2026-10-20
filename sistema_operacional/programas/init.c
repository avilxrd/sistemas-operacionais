#include "init.h"
#include "syscall.h"

int counter_init;

//primeiro programa: fica em loop (apenas para teste)
void init(void)
{
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
