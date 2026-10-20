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
    
    tabela_processos[index_init].estado = EXECUTANDO;
    processo_atual = index_init;

    //retorna o endereco do contexto preparado
    return (int) &tabela_processos[index_init].contexto[0];

}
