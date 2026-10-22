#ifndef PROCESSO_H
#define PROCESSO_H

/* Limite de processos simultaneos. Uma entrada LIVRE pode ser reutilizada. */
#define MAX_PROCESSOS 8

/* NAO AUMENTAR QUANTUM PARA VALORES ACIMA DE 32 
 * OVERFLOW EM void atualiza_priotidade(int index); !! */
#define QUANTUM 3

/* Tamanho de cada pilha em palavras (16 bits). */
#define TAMANHO_PILHA 128

/* Indices no quadro de 16 palavras restaurado por rete. */
#define CTX_SP 6 
#define CTX_IP 7 
#define CTX_SR 8 

/* Estados possiveis do processo. */
#define LIVRE      0 
#define PRONTO     1
#define EXECUTANDO 2
#define BLOQUEADO  3

/* Motivos possiveis para o bloqueio do processo. */
#define BLOQ_NENHUM   0
#define BLOQ_PROCESSO 1
#define BLOQ_LEITURA  2

/* Escalonador */
#define ESC_RR         0
#define ESC_PRIORIDADE 1

#define ESCALONADOR ESC_PRIORIDADE

struct Processo
{
    int pid;               // Id do processo.             
    int estado;            // Estado do processo.          
    int contexto[16];      // Copia dos registradores do processo.    
    int pid_esperado;      // PID alvo quando BLOQ_PROCESSO.    
    int motivo_bloqueio;   // Motivo pelo qual o processo foi bloqueado.
    int prioridade;        // 0 - 1000.
    int registro_metricas; //
};

/* Prepara contexto e pilha e marca PRONTO. Recebe o endereco inicial.
 * Retorna o index na tabela ou -1 se nao houver vaga. Nao entrega a CPU.
 * O endereco recebido ainda nao e validado. */
int cria_processo(int endereco_inicio);

/* Copia 16 palavras do quadro da interrupcao para o processo atual.
 * Nao faz nada se processo_atual == -1. Quadro deve apontar para 16 palavras validas. */
void salva_contexto(int *quadro);

/* Copia o contexto do processo para o quadro da interrupcao.
 * Nao faz nada sem processo atual. */
void restaura_contexto(int *quadro);

/* Mantem o atual se executavel, caso contrario escolhe o primeiro PRONTO.
 * Atualiza processo_atual e o estado e deixa -1 se nao houver candidato.
 * Nao copia contextos. Ainda não é preemptivo. */
void so_escalona(void);

/* Libera a entrada do PID e torna PRONTOS os processos que esperavam por ele.
 * PID 0 significa o proprio processo atual. Retorna 0 para sucesso e -1 para erro.
 * A escolha do proximo processo fica com o chamador. Nao altera processo_atual. */
int mata_processo(int pid);

/* Bloqueia o atual ate a morte do PID informado. Retorna 0 ao registrar
 * a espera, -1 sem processo atual, PID inexistente ou espera por si mesmo.
 * O retorno imediato ao kernel. Usuario so continua depois de ser escalonado.
 * Ciclos ainda nao sao detectados. */
int espera_processo(int pid);

/* Torna um processo pronto e adiciona ele à fila de prontos. 
 * Verifica se ele já está na fila para evitar duplicados.
 * O index que e inserido na fila, nao o PID. */
void torna_pronto(int index);

/* Remove do um processo da fila de prontos.
 * Procura pelo index na fila. Desloca os elementos apos remocao.
 * Decrementa a quantidade de elementos da fila. */
void remove_fila(int index);

/* Atualiza a prioridade de um processo. Recebe o
 * index do processo na tabela de processos. */
void atualiza_prioridade(int index);

/* Modifica o estado de um processo. */
void muda_estado(int index, int novo_estado);

#endif
