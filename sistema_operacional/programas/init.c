#include "init.h"
#include "syscall.h"

/* imprime B e encerra a si mesmo, desbloqueando o init. */
int filho_exec = 0;
void processo_b(void)
{
    so_escreve('B');
    filho_exec = 1;

    so_mata_proc(0);
    
    while (1) {}
}



void init(void)
{
    int pid_b;

    filho_exec = 0;
    pid_b = so_cria_processo((int) processo_b);

    if (pid_b == -1)
    {
        so_escreve('E');
    }
    else 
    {
        while (filho_exec == 0){}
        so_escreve('C');
    }

    so_escreve(10);
    so_mata_proc(0);

    while (1) {}
}
