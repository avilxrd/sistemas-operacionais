#include "init.h"
#include "processo.h"
#include "syscall.h"
#include "metricas.h"

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

    inicializa_metricas();

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


/* Trata o quantum e as operacoes pendentes antes de escolher o proximo. */
int trata_relogio(int *quadro)
{
    registra_interrupcao(10);

    salva_contexto(quadro);

    trata_pendencias();

    if (processo_atual != -1)
    {
        if (tabela_processos[processo_atual].estado == EXECUTANDO &&
            ESCALONADOR != ESC_SIMPLES)
        {
            quantum_restante--;

            if (quantum_restante == 0)
            {
                registra_preempcao(tabela_processos[processo_atual].registro_metricas);
                atualiza_prioridade(processo_atual);
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
 * Retorna 0 para rete, 1 para ociosidade ou -1 para parar a maquina.
 * O resultado e separado (fica em contexto[0]) e o r0 salvo. */
int trata_syscall(int *quadro)
{
    int novo_idx;
    int chamada;
    int argumento;
    int resultado;
    
    int chamador;
    int pid_alvo;

    registra_interrupcao(7);

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

        if (resultado == 0 && pid_alvo == pid_init)
        {
            imprime_metricas();
            return -1;
        }
    }

    // * SYSCALL 4 ||| SO_ESPERA_PROC
    if (chamada == SO_ESPERA_PROC)
    {
        resultado = espera_processo(argumento);
    }

    // * SYSCALL 5 ||| SO_LE
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
            muda_estado(chamador, BLOQUEADO);
            
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

    /* Sem processo pronto, a ponte entra no laco ocioso com interrupcoes. */
    if (processo_atual == -1)
    {
        return 1;
    }

    /* Pode restaurar outro processo diferente do que fez a chamada. */
    restaura_contexto(quadro);
    return 0;
}

/* Conta uma excecao e encerra o processo que a provocou. */
int trata_excecao(int *quadro, int numero)
{
    registra_interrupcao(numero);
    
    if ((quadro[CTX_SR] & 32768) != 0 || processo_atual == -1)
    {
        imprime_metricas();
        return -1;
    }

    salva_contexto(quadro);
    if (tabela_processos[processo_atual].pid == pid_init)
    {
        mata_processo(0);
        imprime_metricas();
        return -1;
    }

    mata_processo(0);
    so_escalona();
    if (processo_atual == -1)
    {
        return 1;
    }
    restaura_contexto(quadro);
    return 0;
}
