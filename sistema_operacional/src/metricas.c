#include "metricas.h"
#include "processo.h"


/* Pontes internas (executadas em modo supervisor) */
void console_escreve(int caractere);


void console_inteiro(int valor);


/* Variaveis Globais */
int total_criados;
int total_preemps;
int interrupcoes[16];

struct MetricaProcesso historico[HISTORICO_MAX];

void inicializa_metricas(void)
{
    total_criados = 0;
    total_preemps = 0;

    for (int i=0; i<16; i++)
    {
        interrupcoes[i] = 0;
    }
}


int registra_criacao(int pid)
{
    int registro;

    if (total_criados == HISTORICO_MAX)
    {
        return -1;
    }

    registro = total_criados;

    historico[registro].pid     = pid;
    historico[registro].estado  = LIVRE;
    historico[registro].preemps = 0;

    for (int i=0; i<4; i++)
    {
        historico[registro].entradas[i] = 0;
    }

    total_criados++;
    return registro;
}


void registra_preempcao(int registro)
{
    total_preemps++;
    historico[registro].preemps++;
}


void registra_estado(int registro, int novo_estado)
{
    if (historico[registro].estado == novo_estado)
    {
        return;
    }

    historico[registro].estado = novo_estado;

    if (novo_estado != LIVRE)
    {
        historico[registro].entradas[novo_estado]++;
    }
}


void registra_interrupcao(int numero)
{
    if (numero >= 0 && numero < 16)
    {
        interrupcoes[numero]++;
    }
}


/* Escreve um texto. */
void metricas_texto(char *texto)
{
    while (*texto != 0)
    {
        console_escreve(*texto);
        texto++;
    }
}


void imprime_metricas(void)
{
    metricas_texto("\nMetricas: \n");

    metricas_texto("\nProcessos criados: ");
    console_inteiro(total_criados);

    metricas_texto("\nPreempcoes por fim de Quantum: ");
    console_inteiro(total_preemps);

    metricas_texto("\nChamadas de Sistema: ");
    console_inteiro(interrupcoes[7]);

    metricas_texto("\nInterrupcoes de Relogio: ");
    console_inteiro(interrupcoes[10]);

    metricas_texto("\nPor processo: \n");
    
    for (int i=0; i<total_criados; i++)
    {
        metricas_texto("PID ");
        console_inteiro(historico[i].pid);
    
        if (historico[i].estado == LIVRE)
        {
            metricas_texto(" encerrado");
        }
        else
        {
            metricas_texto(" ainda ativo");
        }

        metricas_texto(" | preempcoes: ");
        console_inteiro(historico[i].preemps);

        metricas_texto(" | pronto: ");
        console_inteiro(historico[i].entradas[PRONTO]);

        metricas_texto(" | executando: ");
        console_inteiro(historico[i].entradas[EXECUTANDO]);

        metricas_texto(" | bloqueado: ");
        console_inteiro(historico[i].entradas[BLOQUEADO]);

        metricas_texto("\n");
    }

    metricas_texto("\n");
}