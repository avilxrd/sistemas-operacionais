/* Gerenciamento de processos. 
 * Chamadas feitas pelo kernel em modo supervisor.
 * Os handlers atuais mantem interrupcoes externas desabilitadas durante as alteracoes. */
#include "processo.h"

struct Processo tabela_processos[MAX_PROCESSOS];
int processo_atual; // Indice na tabela. -1 significa nenhuma execucao de usuario.
int proximo_pid;    // Sequencia de PIDs.

/* Cada entrada da tabela possui uma pilha exclusiva. */
int pilhas[MAX_PROCESSOS][TAMANHO_PILHA];

/* So fica PRONTO depois que todos os dados foram preparados. */
int cria_processo(int endereco_inicio)
{
    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado == LIVRE)
        {
            tabela_processos[i].pid = proximo_pid;
            proximo_pid++;

            for (int j=0; j<16; j++)
            {
                tabela_processos[i].contexto[j] = 0;
            }

            tabela_processos[i].contexto[CTX_SP] = (int) &pilhas[i][TAMANHO_PILHA];
            tabela_processos[i].contexto[CTX_IP] = endereco_inicio;

            /* S=0 e I=0: usuario sem E/S direta. D=0 permite interrupcoes externas. */
            tabela_processos[i].contexto[CTX_SR] = 0;
            tabela_processos[i].pid_esperado = 0;
            tabela_processos[i].motivo_bloqueio = BLOQ_NENHUM;
            tabela_processos[i].estado = PRONTO;

            return i;
        }
    }

    return -1;
}

/* A pilha da BIOS e reutilizada em cada interrupcao. A tabela preserva o contexto. */
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

/* Prepara o quadro que rete consumira. Nao altera registradores da CPU. */
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

/* Continuar o atual enquanto ele puder executar. */
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
        if (processo_atual == -1)
        {
            return -1;
        }

        pid = tabela_processos[processo_atual].pid;
    }

    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado != LIVRE &&
            tabela_processos[i].pid == pid)
        {
            tabela_processos[i].estado = LIVRE;
            tabela_processos[i].pid_esperado = 0;
            tabela_processos[i].motivo_bloqueio = BLOQ_NENHUM;
            
            /* Acorda apenas quem espera esta morte, outros permanecem bloqueados. */
            for (int j=0; j<MAX_PROCESSOS; j++)
            {
                if (tabela_processos[j].estado == BLOQUEADO              &&
                    tabela_processos[j].motivo_bloqueio == BLOQ_PROCESSO &&
                    tabela_processos[j].pid_esperado == pid)
                {
                    tabela_processos[j].pid_esperado = 0;
                    tabela_processos[j].motivo_bloqueio = BLOQ_NENHUM;
                    tabela_processos[j].contexto[0] = 0;
                    tabela_processos[j].estado = PRONTO;
                }
            }
            return 0;
        }
    }
    return -1;
}

/* Registra a dependencia de processos. O escalonador sera chamado pelo tratamento da syscall. */
int espera_processo(int pid)
{
    if (processo_atual == -1)
    {
        return -1;
    }

    if (pid == tabela_processos[processo_atual].pid)
    {
        return -1;
    }

    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado != LIVRE &&
            tabela_processos[i].pid == pid)
        {
            tabela_processos[processo_atual].pid_esperado = pid;
            tabela_processos[processo_atual].motivo_bloqueio = BLOQ_PROCESSO;
            tabela_processos[processo_atual].estado = BLOQUEADO;
            return 0;
        }
    }
    return -1;
}
