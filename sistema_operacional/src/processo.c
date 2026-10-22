/* Gerenciamento de processos. 
 * Chamadas feitas pelo kernel em modo supervisor.
 * Os handlers atuais mantem interrupcoes externas desabilitadas durante as alteracoes. */
#include "processo.h"
#include "metricas.h"

/* Variaveis globais */
struct Processo tabela_processos[MAX_PROCESSOS];
int processo_atual; // Indice na tabela. -1 significa nenhuma execucao de usuario.
int proximo_pid;    // Sequencia de PIDs.

/* Round-Robin */
int fila_prontos[MAX_PROCESSOS]; // Guarda o index, nao o PID
int quantum_restante;
int quant_prontos;

int pilhas[MAX_PROCESSOS][TAMANHO_PILHA]; // Cada entrada da tabela possui uma pilha exclusiva.


void remove_fila(int index)
{
    for (int i=0; i<quant_prontos; i++)
    {
        if (fila_prontos[i] == index)
        {
            for (int j=i; j<quant_prontos-1; j++)
            {
                fila_prontos[j] = fila_prontos[j+1];
            }
        
            quant_prontos--;
            return;
        }
    }
}


void muda_estado(int index, int novo_estado)
{
    registra_estado(tabela_processos[index].registro_metricas, novo_estado);
    tabela_processos[index].estado = novo_estado;
}


void torna_pronto(int index)
{
    for (int i=0; i<quant_prontos; i++)
    {
        if (fila_prontos[i] == index)
        {
            return;
        }
    }

    muda_estado(index, PRONTO);
    fila_prontos[quant_prontos] = index;
    quant_prontos++;

}

/* So fica PRONTO depois que todos os dados foram preparados. */
int cria_processo(int endereco_inicio)
{
    int registro;

    if (endereco_inicio == 0)
    {
        return -1;
    }

    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado == LIVRE)
        {
            registro = registra_criacao(proximo_pid);

            if (registro == -1)
            {
                return -1;
            }

            tabela_processos[i].registro_metricas = registro;
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
            tabela_processos[i].prioridade = 500;

            torna_pronto(i);
            
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


void atualiza_prioridade(int index)
{
    int tempo_usado;
    int fracao;

    tempo_usado = QUANTUM - quantum_restante;
    fracao = (tempo_usado * 1000) / QUANTUM;

    tabela_processos[index].prioridade = (tabela_processos[index].prioridade + fracao)/2;
}



/* Continuar o atual enquanto ele puder executar. */
void so_escalona(void)
{
    int escolhido;

    if (processo_atual != -1)
    {
        if (tabela_processos[processo_atual].estado == EXECUTANDO)
        {
            return;
        }

        if (tabela_processos[processo_atual].estado == BLOQUEADO)
        {
            atualiza_prioridade(processo_atual);
        }
    }

    processo_atual = -1;

    if (quant_prontos == 0)
    {
        registra_ociosidade(1);
        return;
    }

    escolhido = fila_prontos[0];

    if (ESCALONADOR == ESC_PRIORIDADE)
    {
        for (int i=1; i<quant_prontos; i++)
        {
            if (tabela_processos[fila_prontos[i]].prioridade < 
                tabela_processos[escolhido].prioridade)
            {
                escolhido = fila_prontos[i];
            }
        }
    }

    if (ESCALONADOR == ESC_SIMPLES)
    {
        for (int i=0; i<MAX_PROCESSOS; i++)
        {
            if (tabela_processos[i].estado == PRONTO)
            {
                escolhido = i;
                break;
            }
        }
    }

    registra_ociosidade(0);
    processo_atual = escolhido;
    remove_fila(processo_atual);

    muda_estado(processo_atual, EXECUTANDO);
    quantum_restante = QUANTUM;

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
            remove_fila(i);
            muda_estado(i, LIVRE);
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
                    torna_pronto(j);
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
            muda_estado(processo_atual, BLOQUEADO);
            return 0;
        }
    }
    return -1;
}
