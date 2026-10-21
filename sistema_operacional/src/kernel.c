#include "init.h"
#include "processo.h"
#include "syscall.h"

void console_escreve(int caractere);

int pid_init;

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

    pid_init = tabela_processos[index_init].pid;

    so_escalona();

    if (processo_atual == -1)
    {
        return -1;
    }

    return (int) &tabela_processos[processo_atual].contexto[0];
}

int trata_syscall(int *quadro)
{
    int novo_idx;
    int chamada;
    int argumento;
    int resultado;
    
    int chamador;
    int pid_alvo;

    salva_contexto(quadro);

    chamador = processo_atual;

    chamada   = quadro[0]; //r0: numero da chamada
    argumento = quadro[1]; //r1: primeiro argumento

    resultado = -1;

    if (chamada == SO_ESCREVE)
    {
        console_escreve(argumento);
        resultado = 0;
    }
    if (chamada == SO_CRIA_PROC)
    {
        novo_idx = cria_processo(argumento);
        
        if (novo_idx != -1)
        {
            resultado = tabela_processos[novo_idx].pid;
        }
    }
    if (chamada == SO_MATA_PROC)
    {
        pid_alvo = argumento;

        if (pid_alvo == 0)
        {
            pid_alvo = tabela_processos[chamador].pid;
        }

        resultado = mata_processo(pid_alvo);

        if (resultado == 0 && pid_alvo == pid_init)
        {
            return -1;
        }
    }
    if (chamada == SO_ESPERA_PROC)
    {
        resultado = espera_processo(argumento);
    }

    if (tabela_processos[chamador].estado != LIVRE)
    {
        tabela_processos[chamador].contexto[0] = resultado;
    }
    
    so_escalona();

    if (processo_atual == -1)
    {
        return -1;
    }

    restaura_contexto(quadro);
    return 0;
}   