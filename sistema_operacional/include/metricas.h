#ifndef METRICAS_H
#define METRICAS_H

#define HISTORICO_MAX 64

/* Um bloco vale 10000 instrucoes. Evita estourar um int de 16 bits. */
struct Tempo
{
    int blocos;
    int unidades;
};

struct MetricaProcesso
{
    int pid;
    int estado;
    int preemps;
    int entradas[4]; // LIVRE | PRONTO | EXECUTANDO | BLOQUEADO
    struct Tempo tempos[4];
    struct Tempo inicio_estado;
    struct Tempo criacao;
    struct Tempo retorno;
    struct Tempo inicio_resposta;
    struct Tempo resposta_total;
    int respostas;
    int aguardando_resposta;
};

/* Operacoes sobre tempos. Os argumentos de saida recebem o resultado. */
void tempo_zera(struct Tempo *tempo);
void tempo_soma(struct Tempo *tempo, int incremento);
void tempo_copia(struct Tempo *destino, struct Tempo *origem);
void tempo_diferenca(struct Tempo *resultado, struct Tempo *fim, struct Tempo *inicio);
void tempo_acumula(struct Tempo *destino, struct Tempo *valor);
void tempo_media(struct Tempo *resultado, struct Tempo *total, int quantidade);
void tempo_imprime(struct Tempo *tempo);

/* Pontes internas. Exigem supervisor e interrupcoes desabilitadas. */
int relogio_contador(void);
void relogio_inicia(int periodo);

/* Inicializa as metricas e inicia o relogio. */
void inicializa_metricas(void);

/* Atualiza o tempo pela leitura do contador e pelas voltas do relogio.
 * numero == 10 indica atendimento do relogio; -1 indica somente leitura. */
void atualiza_tempo(int numero);

/* Conta uma criacao. Retorna o indice no historico ou -1 se estiver cheio. */
int registra_criacao(int pid);
void registra_preempcao(int registro);
void contabiliza_estado(int registro);
void registra_estado(int registro, int novo_estado);
void registra_interrupcao(int numero);
void registra_ociosidade(int ocioso);

/* Fecha os intervalos antes de imprimir. A impressao nao entra nos tempos. */
void finaliza_metricas(void);
void imprime_metricas(void);

#endif
