#ifndef METRICAS_H
#define METRICAS_H

#define HISTORICO_MAX 64

/* Gambiarra para calcular o tempo pois os 
 * 16 bits de int estouram muito rapido */
struct Tempo
{
    int blocos;
    int unidades;
};

void tempo_zera(struct Tempo *tempo);

void tempo_soma(struct Tempo *tempo, int incremento);

int relogio_contador(void);

struct MetricaProcesso
{
    int pid;
    int estado;
    int preemps;
    int entradas[4]; // LIVRE | PRONTO | BLOQUEADO | EXECUTANDO
    struct Tempo tempos[4];
    struct Tempo inicio_estado;
};

/* Zera os contadores antes da criacao do init. */
void inicializa_metricas(void);

/* Conta uma criacao concluida com sucesso. */
int registra_criacao(int pid);

/* Conta uma retirada da CPU por fim de Quantum */
void registra_preempcao(int registro);

/*  */
void registra_estado(int registro, int novo_estado);

/* Conta uma interrupcao atendida. Recebe o numero do
 * interrupcao. */
void registra_interrupcao(int numero);

/* Imprime a parte das metricas que calcula o tempo */
void tempo_imprime(struct Tempo *tempo);

/* Imprime no console do simulador os valores capturados
 * de metricas. */
void imprime_metricas(void);


#endif