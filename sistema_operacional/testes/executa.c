/* Executa o nucleo original do simulador sem curses.
 * A entrada chega em uma instrucao determinada, tornando os testes repetiveis. */
#include "cpu.h"
#include "objeto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 2) return 2;
    mem_t *mem = mem_cria();
    disp_t *disp = disp_cria(mem);
    cpu_t *cpu = cpu_cria(mem, disp);
    simbolo_t *simbolos = NULL;
    char erro[256];
    if (!obj_carrega(argv[1], mem, &simbolos, erro, sizeof erro))
    {
        fprintf(stderr, "%s\n", erro);
        return 2;
    }
    bool achou;
    uint16_t leitura = obj_busca_simbolo(simbolos, "relogio_le_byte_baixo", &achou);
    uint16_t ler = obj_busca_simbolo(simbolos, "_f_relogio_contador", &achou);
    uint16_t soma = obj_busca_simbolo(simbolos, "_f_tempo_soma", &achou);
    uint16_t total = obj_busca_simbolo(simbolos, "_g_tempo_total", &achou);
    uint16_t ultimo = obj_busca_simbolo(simbolos, "_g_ultimo_contador", &achou);
    uint16_t orig_ret = 0;
    long origem = -1, amostra = -1, verificacoes = 0, falhas = 0, erro_max = 0;
    if (!achou) { fprintf(stderr, "simbolos de teste ausentes\n"); return 2; }
    long injeta = argc > 2 ? atol(argv[2]) : -1;
    cpu_liga(cpu);
    while (!cpu_parada(cpu) && cpu_num_instrucoes(cpu) < 12000000)
    {
        long passo = cpu_num_instrucoes(cpu);
        uint16_t ip = cpu_r(cpu, 7);
        if (passo == injeta)
        {
            const char *texto = argc > 3 ? argv[3] : "x";
            while (*texto) console_poe_entrada(disp, *texto++);
        }
        if (ip == ler && origem < 0) orig_ret = mem_le_palavra(mem, cpu_r(cpu, 6));
        if (ip == leitura) amostra = passo;
        if (ip == orig_ret && origem < 0 && amostra >= 0) origem = amostra;
        if (ip == soma && origem >= 0)
        {
            uint16_t sp = cpu_r(cpu, 6);
            if (mem_le_palavra(mem, sp + 2) == total)
            {
                long registrado = mem_le_palavra(mem, total) * 10000L
                                 + mem_le_palavra(mem, total + 2)
                                 + mem_le_palavra(mem, sp + 4);
                long diferenca = registrado - (amostra - origem);
                verificacoes++;
                if (diferenca != 0)
                {
                    falhas++;
                    if (labs(diferenca) > erro_max) erro_max = labs(diferenca);
                    if (falhas < 4) fprintf(stderr, "tempo divergente: passo=%ld erro=%ld contador=%u\n", passo, diferenca, mem_le_palavra(mem, ultimo));
                }
            }
        }
        cpu_executa_1(cpu);
        int n;
        const char *saida = console_saida(disp, &n);
        if (n)
        {
            fwrite(saida, 1, n, stdout);
            console_limpa_saida(disp);
        }
    }
    fprintf(stderr, "passos=%ld amostras=%ld falhas_relogio=%ld erro_max=%ld halt=%d\n",
            cpu_num_instrucoes(cpu), verificacoes, falhas, erro_max, cpu_parada(cpu));
    int resultado = !cpu_parada(cpu) || falhas ? 1 : 0;
    obj_libera_simbolos(simbolos);
    cpu_destroi(cpu);
    disp_destroi(disp);
    mem_destroi(mem);
    return resultado;
}
