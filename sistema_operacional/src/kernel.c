#include "processo.h"

struct Processo tabela_processos[MAX_PROCESSOS];

int processo_atual;
int proximo_pid;

int pilha_init[128];
int counter_init;


//
void init(void)
{
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


//
int cria_processo()
{
    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado == LIVRE)
        {
            tabela_processos[i].pid = proximo_pid;
            proximo_pid++;

            //inicializa todos os registradores como 0
            for (int j=0; j<16; j++)
            {
                tabela_processos[i].contexto[j] = 0;
            }

            //retorna o indice do processo na tabela de processos
            return i;
        }
    }
    //retorna -1 se a tabela estiver cheia
    return -1;
}


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

    index_init = cria_processo();
    if (index_init == -1)
    {
        return -1;
    }

    tabela_processos[index_init].contexto[6] = (int)&pilha_init[128];
    tabela_processos[index_init].contexto[7] = (int)init;
    
    tabela_processos[index_init].estado = EXECUTANDO;
    processo_atual = index_init;

    //retorna o endereco do contexto preparado
    return (int) &tabela_processos[index_init].contexto[0];

}
