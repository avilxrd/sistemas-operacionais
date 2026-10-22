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

struct Tempo tempo_periodos;

void tempo_zera(struct Tempo *tempo)
{
    tempo->blocos   = 0;
    tempo->unidades = 0;
}


void tempo_soma(struct Tempo *tempo, int incremento)
{
    tempo->blocos   += incremento / 10000;
    tempo->unidades += incremento % 10000;

    if (tempo->unidades >= 10000)
    {
        tempo->unidades -= 10000;
        tempo->blocos++;
    }
}


void inicializa_metricas(void)
{
    total_criados = 0;
    total_preemps = 0;

    for (int i=0; i<16; i++)
    {
        interrupcoes[i] = 0;
    }

    tempo_zera(&tempo_periodos);
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
        tempo_zera(&historico[registro].tempos[i]);
    }
    historico[registro].inicio_estado.blocos = tempo_periodos.blocos;
    historico[registro].inicio_estado.unidades = tempo_periodos.unidades;
    
    total_criados++;
    return registro;
}


void registra_preempcao(int registro)
{
    total_preemps++;
    historico[registro].preemps++;
}


void contabiliza_estado(int registro)
{
    int estado;
    int blocos;
    int unidades;

    estado = historico[registro].estado;

    blocos = tempo_periodos.blocos - historico[registro].inicio_estado.blocos;

    unidades = tempo_periodos.unidades - historico[registro].inicio_estado.unidades;

    if (unidades < 0)
    {
        unidades += 10000;
        blocos--;
    }

    if (estado != LIVRE)
    {
        historico[registro].tempos[estado].blocos += blocos;
        tempo_soma(&historico[registro].tempos[estado], unidades);
    }

    historico[registro].inicio_estado.blocos = tempo_periodos.blocos;
    historico[registro].inicio_estado.unidades = tempo_periodos.unidades;
}


void registra_estado(int registro, int novo_estado)
{
    if (historico[registro].estado == novo_estado)
    {
        return;
    }

    contabiliza_estado(registro);

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

    if (numero == 10)
    {
        tempo_soma(&tempo_periodos, 4000);
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


void tempo_imprime(struct Tempo *tempo)
{
    
    if (tempo->blocos == 0)
    {
        console_inteiro(tempo->unidades);
        return;
    }

    console_inteiro(tempo->blocos);

    if (tempo->unidades < 1000)
    {
        console_escreve('0');
    }

    if (tempo->unidades < 100)
    {
        console_escreve('0');
    }

    if (tempo->unidades < 10)
    {
        console_escreve('0');
    }

    console_inteiro(tempo->unidades);
}


void imprime_metricas(void)
{
    metricas_texto("\nMetricas: ");

    metricas_texto("\nProcessos criados: ");
    console_inteiro(total_criados);

    metricas_texto("\nPreempcoes por fim de Quantum: ");
    console_inteiro(total_preemps);

    metricas_texto("\nChamadas de Sistema: ");
    console_inteiro(interrupcoes[7]);

    metricas_texto("\nInterrupcoes de Relogio: ");
    console_inteiro(interrupcoes[10]);

    metricas_texto("\nPeriodos atendidos (em instrucoes): ");
    tempo_imprime(&tempo_periodos);

    metricas_texto("\n\nPor processo: \n");
    
    for (int i=0; i<total_criados; i++)
    {
        contabiliza_estado(i);

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

        metricas_texto("\nTempo pronto: ");
        tempo_imprime(&historico[i].tempos[PRONTO]);

        metricas_texto(" | executando: ");
        tempo_imprime(&historico[i].tempos[EXECUTANDO]);

        metricas_texto(" | bloqueado: ");
        tempo_imprime(&historico[i].tempos[BLOQUEADO]);

        metricas_texto("\n");
    }

    metricas_texto("\n");
}