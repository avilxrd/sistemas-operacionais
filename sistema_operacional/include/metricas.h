#ifndef METRICAS_H
#define METRICAS_H

#define HISTORICO_MAX 64

struct  MetricaProcesso
{
    int pid;
    int estado;
    int preemps;
    int entradas[4]; // LIVRE | PRONTO | BLOQUEADO | EXECUTANDO
};


/* Zera os contadores antes da criacao do init. */
void inicializa_metricas(void);


/* Conta uma criacao concluida com sucesso.
 * Conta uma retirada da CPU por fim de Quantum */
void registra_estado(int registro, int novo_estado);


/* Conta uma interrupcao atendida. Recebe o numero do
 * interrupcao. */
void registra_interrupcao(int numero);


/* Imprime no console do simulador os valores capturados
 * de metricas. */
void imprime_metricas(void);


#endif