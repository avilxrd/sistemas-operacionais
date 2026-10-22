#include "metricas.h"
#include "processo.h"

/* Pontes internas (executadas em modo supervisor). */
void console_escreve(int caractere);
void console_inteiro(int valor);

/* Variaveis globais. O historico nao e apagado quando uma vaga e reutilizada. */
int total_criados;
int total_preemps;
int interrupcoes[16];
struct MetricaProcesso historico[HISTORICO_MAX];
struct Tempo tempo_total;
struct Tempo tempo_ocioso;
int ultimo_contador;
int voltas_pendentes;
int sistema_ocioso;
int metricas_finalizadas;


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


void tempo_copia(struct Tempo *destino, struct Tempo *origem)
{
    destino->blocos   = origem->blocos;
    destino->unidades = origem->unidades;
}


void tempo_diferenca(struct Tempo *resultado, struct Tempo *fim, struct Tempo *inicio)
{
    resultado->blocos   = fim->blocos   - inicio->blocos;
    resultado->unidades = fim->unidades - inicio->unidades;

    if (resultado->unidades < 0)
    {
        resultado->unidades += 10000;
        resultado->blocos--;
    }
}


void tempo_acumula(struct Tempo *destino, struct Tempo *valor)
{
    destino->blocos += valor->blocos;
    tempo_soma(destino, valor->unidades);
}


void tempo_media(struct Tempo *resultado, struct Tempo *total, int quantidade)
{
    int resto;
    int novo_resto;
    int digito;
    int algarismo;
    int divisor;

    tempo_zera(resultado);
    if (quantidade == 0)
    {
        return;
    }

    resultado->blocos = total->blocos / quantidade;
    resto = total->blocos % quantidade;
    divisor = 1000;

    for (int i=0; i<4; i++)
    {
        novo_resto = 0;
        digito = 0;
        
        for (int j=0; j<10; j++)
        {
            if (novo_resto >= quantidade - resto)
            {
                novo_resto -= quantidade - resto;
                digito++;
            }
            else
            {
                novo_resto += resto;
            }
        }

        algarismo = (total->unidades / divisor) % 10;
        for (int j=0; j<algarismo; j++)
        {
            novo_resto++;
            if (novo_resto == quantidade)
            {
                novo_resto = 0;
                digito++;
            }
        }

        resultado->unidades = resultado->unidades * 10 + digito;
        resto = novo_resto;
        divisor /= 10;
    }
}


void atualiza_tempo(int numero)
{
    int contador;
    int intervalo;

    if (metricas_finalizadas)
    {
        return;
    }

    contador = relogio_contador();
    intervalo = contador - ultimo_contador;

    if (intervalo < 0)
    {
        intervalo += PERIODO_RELOGIO;
        voltas_pendentes++;
    }

    if (numero == 10)
    {
        if (voltas_pendentes != 0)
        {
            voltas_pendentes--;
        }
        else
        {
            intervalo += PERIODO_RELOGIO;
        }
    }

    ultimo_contador = contador;
    tempo_soma(&tempo_total, intervalo);
    if (sistema_ocioso)
    {
        tempo_soma(&tempo_ocioso, intervalo);
    }
}


void inicializa_metricas(void)
{
    total_criados = 0;
    total_preemps = 0;
    voltas_pendentes = 0;
    sistema_ocioso = 0;
    metricas_finalizadas = 0;

    for (int i=0; i<16; i++)
    {
        interrupcoes[i] = 0;
    }
    interrupcoes[0] = 1;

    tempo_zera(&tempo_total);
    tempo_zera(&tempo_ocioso);
    relogio_inicia(PERIODO_RELOGIO);
    ultimo_contador = relogio_contador();
}


int registra_criacao(int pid)
{
    int registro;

    if (total_criados == HISTORICO_MAX)
    {
        return -1;
    }

    registro = total_criados;
    historico[registro].pid = pid;
    historico[registro].estado = LIVRE;
    historico[registro].preemps = 0;
    historico[registro].respostas = 0;
    historico[registro].aguardando_resposta = 0;
    tempo_zera(&historico[registro].retorno);
    tempo_zera(&historico[registro].resposta_total);
    tempo_zera(&historico[registro].inicio_resposta);

    for (int i=0; i<4; i++)
    {
        historico[registro].entradas[i] = 0;
        tempo_zera(&historico[registro].tempos[i]);
    }

    atualiza_tempo(-1);
    tempo_copia(&historico[registro].criacao, &tempo_total);
    tempo_copia(&historico[registro].inicio_estado, &tempo_total);
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
    struct Tempo intervalo;
    int estado;

    estado = historico[registro].estado;
    tempo_diferenca(&intervalo, &tempo_total, &historico[registro].inicio_estado);

    if (estado != LIVRE)
    {
        tempo_acumula(&historico[registro].tempos[estado], &intervalo);
    }
    tempo_copia(&historico[registro].inicio_estado, &tempo_total);
}


void registra_estado(int registro, int novo_estado)
{
    struct Tempo intervalo;
    int anterior;

    anterior = historico[registro].estado;
    if (anterior == novo_estado)
    {
        return;
    }

    atualiza_tempo(-1);
    contabiliza_estado(registro);

    if (anterior == LIVRE && novo_estado == PRONTO)
    {
        tempo_copia(&historico[registro].criacao, &tempo_total);
    }

    if (anterior == BLOQUEADO && novo_estado == PRONTO)
    {
        tempo_copia(&historico[registro].inicio_resposta, &tempo_total);
        historico[registro].aguardando_resposta = 1;
    }

    if (novo_estado == EXECUTANDO && historico[registro].aguardando_resposta)
    {
        tempo_diferenca(&intervalo, &tempo_total, &historico[registro].inicio_resposta);
        tempo_acumula(&historico[registro].resposta_total, &intervalo);
        historico[registro].respostas++;
        historico[registro].aguardando_resposta = 0;
    }

    if (novo_estado == LIVRE)
    {
        tempo_diferenca(&historico[registro].retorno, &tempo_total, &historico[registro].criacao);
        historico[registro].aguardando_resposta = 0;
    }
    else
    {
        historico[registro].entradas[novo_estado]++;
    }
    historico[registro].estado = novo_estado;
}


void registra_interrupcao(int numero)
{
    atualiza_tempo(numero);
    if (numero >= 0 && numero < 16)
    {
        interrupcoes[numero]++;
    }
}


void registra_ociosidade(int ocioso)
{
    atualiza_tempo(-1);
    sistema_ocioso = ocioso;
}


void finaliza_metricas(void)
{
    if (metricas_finalizadas)
    {
        return;
    }

    atualiza_tempo(-1);
    metricas_finalizadas = 1;
    for (int i=0; i<total_criados; i++)
    {
        contabiliza_estado(i);
    }
}


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
    struct Tempo media;

    finaliza_metricas();
    metricas_texto("\nMetricas (tempos em instrucoes):");
    metricas_texto("\nProcessos criados: ");
    console_inteiro(total_criados);
    metricas_texto(" | preempcoes: ");
    console_inteiro(total_preemps);
    metricas_texto("\nTempo total: ");
    tempo_imprime(&tempo_total);
    metricas_texto(" | ocioso: ");
    tempo_imprime(&tempo_ocioso);

    metricas_texto("\nInterrupcoes [tipo:quantidade]:");
    for (int i=0; i<16; i++)
    {
        if (i == 8)
        {
            metricas_texto("\n");
        }
        console_escreve(' ');
        console_inteiro(i);
        console_escreve(':');
        console_inteiro(interrupcoes[i]);
    }

    metricas_texto("\nPor processo (P=pronto E=executando B=bloqueado):\n");
    for (int i=0; i<total_criados; i++)
    {
        metricas_texto("PID ");
        console_inteiro(historico[i].pid);
        if (historico[i].estado == LIVRE)
        {
            metricas_texto(" encerrado | retorno: ");
            tempo_imprime(&historico[i].retorno);
        }
        else
        {
            metricas_texto(" ativo | retorno: pendente");
        }
        metricas_texto(" | preempcoes: ");
        console_inteiro(historico[i].preemps);
        metricas_texto("\n  Entradas P/E/B: ");
        console_inteiro(historico[i].entradas[PRONTO]);
        console_escreve('/');
        console_inteiro(historico[i].entradas[EXECUTANDO]);
        console_escreve('/');
        console_inteiro(historico[i].entradas[BLOQUEADO]);
        metricas_texto(" | tempos P/E/B: ");
        tempo_imprime(&historico[i].tempos[PRONTO]);
        console_escreve('/');
        tempo_imprime(&historico[i].tempos[EXECUTANDO]);
        console_escreve('/');
        tempo_imprime(&historico[i].tempos[BLOQUEADO]);
        metricas_texto("\n  Resposta media: ");
        if (historico[i].respostas == 0)
        {
            metricas_texto("n/a");
        }
        else
        {
            tempo_media(&media, &historico[i].resposta_total, historico[i].respostas);
            tempo_imprime(&media);
        }
        metricas_texto(" | respostas concluidas: ");
        console_inteiro(historico[i].respostas);
        console_escreve(10);
    }
}
