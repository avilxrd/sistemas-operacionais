#include "processo.h"

struct Processo tabela_processos[MAX_PROCESSOS];
int processo_atual;
int proximo_pid;

int pilhas[MAX_PROCESSOS][TAMANHO_PILHA];

//procura uma entrada livre e prepara seu PID e contexto
int cria_processo(int endereco_inicio)
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

            tabela_processos[i].contexto[CTX_SP] = (int) &pilhas[i][TAMANHO_PILHA];
            tabela_processos[i].contexto[CTX_IP] = endereco_inicio;
            tabela_processos[i].contexto[CTX_SR] = 0;
            tabela_processos[i].estado = PRONTO;

            //retorna o indice do processo na tabela de processos
            return i;
        }
    }
    
    //retorna -1 se a tabela estiver cheia
    return -1;
}


void salva_contexto(int *quadro)
{
    if (processo_atual == -1)
    {
        return;
    }

    for (int i=0; i<16; i++)
    {
        tabela_processos[processo_atual].contexto[i] = quadro[i];
    }
}


void restaura_contexto(int *quadro)
{
    if (processo_atual == -1)
    {
        return;
    }

    for (int i=0; i<16; i++)
    {
        quadro[i] = tabela_processos[processo_atual].contexto[i];
    }
}


//procura, escolhe o processo e altera o seu estado
void so_escalona()
{
    if (processo_atual != -1)
    {
        if (tabela_processos[processo_atual].estado == EXECUTANDO)
        {
            return;
        }

        if (tabela_processos[processo_atual].estado == PRONTO)
        {
            tabela_processos[processo_atual].estado = EXECUTANDO;
            return;
        }
    }

    processo_atual = -1;

    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado == PRONTO)
        {
            processo_atual = i;
            tabela_processos[i].estado = EXECUTANDO;
            return;
        }
    }
}


int mata_processo(int pid)
{
    if (pid == 0)
    {
        if (processo_atual != -1)
        {
            return 1;
        }

        pid = tabela_processos[processo_atual].pid;
    }

    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado != LIVRE &&
            tabela_processos[i].pid == pid)
        {
            tabela_processos[i].estado = LIVRE;
            return 0;
        }
    }

    return -1;
}