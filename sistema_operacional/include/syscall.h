#ifndef SYSCALL_H
#define SYSCALL_H

/* Numeros enviados em r0 antes de trap 7. Iguais em ponte.asm. */
#define SO_ESCREVE     1
#define SO_CRIA_PROC   2
#define SO_MATA_PROC   3
#define SO_ESPERA_PROC 4
#define SO_LE          5

/* Escreve um caractere no console. Retorna 0.
 * Escreve imediatamente, sem bloqueio. */
int so_escreve(int caractere);

/* Cria um processo a partir de uma funcao existente.
 * Retorna o PID (nao o indice) ou -1 se a tabela estiver cheia. */
int so_cria_processo(int endereco_inicio);

/* Mata o processo com o PID informado. 0 significa o proprio processo.
 * Retorna 0 em caso de sucesso ou -1 para PID inexistente. Nao retorna ao
 * processo encerrado. Encerrar o init faz o kernel parar a maquina. */
int so_mata_proc(int pid);

/* Espera o termino de outro processo existente. Retorna 0 apos a espera
 * ou -1 se o PID nao existir ou for o proprio chamador. */
int so_espera_proc(int pid);

/* Le um caractere do console (com bloqueio). Quando não houver mais caractere
 * libera a CPU. */
int so_le(void);

#endif
