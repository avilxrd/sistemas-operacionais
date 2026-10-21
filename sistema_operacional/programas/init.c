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
    int resultado;

    pid_b = so_cria_processo((int) processo_b);

    if (pid_b == -1)
    {
        so_escreve('E');
    }
    else
    {
        so_escreve('C');

        resultado = so_mata_proc(pid_b);

        if (resultado == 0)
        {
            so_escreve('M');
        }
        else
        {
            so_escreve('E');
        }

        resultado = so_mata_proc(pid_b);

        if (resultado == -1)
        {
            so_escreve('X');
        }
        else
        {
            so_escreve('E');
        }
    }

    while(1){}
}
