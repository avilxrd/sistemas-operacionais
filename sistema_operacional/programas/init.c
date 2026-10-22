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
    int caractere;

    so_escreve('>');

    caractere = so_le();

    so_escreve(caractere);
    so_escreve(10);

    so_mata_proc(0);

    while (1) {}
}
