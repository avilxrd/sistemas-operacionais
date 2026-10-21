#include "init.h"
#include "syscall.h"

/* imprime B e encerra a si mesmo, desbloqueando o init. */
void processo_b(void)
{
    so_escreve('B');
    so_mata_proc(0);
    
    while (1) {}
}

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
        so_escreve('A');

        resultado = so_espera_proc(pid_b);

        if (resultado == 0)
        {
            so_escreve('C');
        }
        else
        {
            so_escreve('E');
        }
    }

    so_escreve(10);
    so_mata_proc(0);

    while (1) {}
}
