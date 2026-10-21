#include "init.h"
#include "processo.h"
#include "syscall.h"

void console_escreve(int caractere);


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

int trata_syscall(int *quadro)
{
    int chamada;
    int argumento;
    int resultado;

    salva_contexto(quadro);

    chamada   = quadro[0]; //r0: numero da chamada
    argumento = quadro[1]; //r1: primeiro argumento

    resultado = -1;

    if (chamada == SO_ESCREVE)
    {
        console_escreve(argumento);
        resultado = 0;
    }

    tabela_processos[processo_atual].contexto[0] = resultado;
    so_escalona();

    if (processo_atual == -1)
    {
        return -1;
    }

    restaura_contexto(quadro);
    return 0;
}