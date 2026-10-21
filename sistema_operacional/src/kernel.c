#include "init.h"
#include "processo.h"
#include "syscall.h"

/* Pontes em assembly para a BIOS. Uso interno em modo supervisor, sem trap. */
void console_escreve(int caractere);

int console_disponivel(void);

int console_le(void);

int pid_init;

/* Inicializa a tabela e cria o init. Retorna o endereco do contexto inicial,
 * ou -1 se nao puder iniciar. */
int kernel_inicio(void)
{
    processo_atual = -1;
    proximo_pid    =  1;
    quant_prontos = 0;
    quantum_restante = 0;

    int index_init;

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


/* Entrega caracteres aos processos bloqueados por leitura. Grava o
 * caractere no r0 salvo e torna o leitor PRONTO. */
void trata_pendencias(void)
{
    for (int i=0; i<MAX_PROCESSOS; i++)
    {
        if (tabela_processos[i].estado == BLOQUEADO &&
            tabela_processos[i].motivo_bloqueio == BLOQ_LEITURA)
        {
            if (console_disponivel() == 0)
            {
                return;
            }

            tabela_processos[i].contexto[0] = console_le();
            tabela_processos[i].motivo_bloqueio = BLOQ_NENHUM;
            torna_pronto(i);
        }
    }
}


/* */
int trata_relogio(int *quadro)
{
    salva_contexto(quadro);

    trata_pendencias();
    so_escalona();

    if (processo_atual != -1)
    {
        if (tabela_processos[processo_atual].estado == EXECUTANDO)
        {
            quantum_restante--;

            if (quantum_restante == 0)
            {
                torna_pronto(processo_atual);
                processo_atual = -1;
            }
        }
    }

    so_escalona();

    if (processo_atual == -1)
    {
        return 1;
    }

    restaura_contexto(quadro);

    return 0;
}


/* Recebe o quadro salvo pela CPU no trap 7, com um processo atual valido.
 * Retorna 0 para a ponte executar rete ou -1 para parar a maquina.
 * O resultado e separado (fica em contexto[0]) e o r0 salvo. */
int trata_syscall(int *quadro)
{
    int novo_idx;
    int chamada;
    int argumento;
    int resultado;
    
    int chamador;
    int pid_alvo;

    salva_contexto(quadro);

    /* Guarda a entrada antes do escalonamento. */
    chamador = processo_atual;

    chamada   = quadro[0]; // r0 salvo: numero do servico. 
    argumento = quadro[1]; // r1 salvo: primeiro argumento. 

    resultado = -1;        // desconhecido ou sem sucesso.

    // * SYSCALL 1 ||| SO_ESCREVE
    if (chamada == SO_ESCREVE)
    {
        console_escreve(argumento);
        resultado = 0;
    }
    
    // * SYSCALL 2 ||| SO_CRIA_PROC
    if (chamada == SO_CRIA_PROC)
    {
        novo_idx = cria_processo(argumento);
        
        if (novo_idx != -1)
        {
            /* Devolve PID e nao o index */
            resultado = tabela_processos[novo_idx].pid;
        }
    }

    // * SYSCALL 3 ||| SO_MATA_PROC
    if (chamada == SO_MATA_PROC)
    {
        pid_alvo = argumento;

        if (pid_alvo == 0)
        {
            pid_alvo = tabela_processos[chamador].pid;
        }

        resultado = mata_processo(pid_alvo);

        /* TODO: implementar a apresentacao de metricas */
        if (resultado == 0 && pid_alvo == pid_init)
        {
            return -1;
        }
    }

    // * SYSCALL 4 ||| SO_ESPERA_PROC
    if (chamada == SO_ESPERA_PROC)
    {
        resultado = espera_processo(argumento);
    }

    // * SYSCALL 5 ||| 
    if (chamada == SO_LE)
    {
        if (console_disponivel() != 0)
        {
            resultado = console_le();
        }
        else
        {
            tabela_processos[chamador].motivo_bloqueio = BLOQ_LEITURA;
            tabela_processos[chamador].pid_esperado     = 0;
            tabela_processos[chamador].estado          = BLOQUEADO;

            resultado = 0;
        }
    }

    /* Nao retorna a quem se encerrou. Bloqueado guarda
     * o resultado para quando voltar a executar. 
     * O rete recupera r0. */
    if (tabela_processos[chamador].estado != LIVRE)
    {
        tabela_processos[chamador].contexto[0] = resultado;
    }
    
    so_escalona();

    // Obs: por enquanto todos bloqueados causam parada.
    if (processo_atual == -1)
    {
        return 1;
    }

    /* Pode restaurar outro processo diferente do que fez a chamada. */
    restaura_contexto(quadro);
    return 0;
}
