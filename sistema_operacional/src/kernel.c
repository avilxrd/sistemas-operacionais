#include "processo.h"
#include "init.h"

//ponto de entrada do kernel
int kernel_inicio(void)
{
    processo_atual = -1;
    proximo_pid    =  1;

    int index_init;

    //inicializa uma tabela de processos vazia
    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        tabela_processos[i].pid    = 0;
        tabela_processos[i].estado = LIVRE;
    }

    index_init = cria_processo((int)init);

    if (index_init == -1)
    {
        return -1;
    }

    so_escalona();

    if (processo_atual == -1)
    {
        return -1;
    }

    return (int) &tabela_processos[processo_atual].contexto[0];
}
