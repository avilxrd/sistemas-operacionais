#include "init.h"
#include "syscall.h"

void filho_b(void)
{
    int soma;
    soma = 0;
    for (int i=0; i<1800; i++) soma = (soma + i) % 100;
    so_escreve('B');
    so_mata_proc(0);
    while (1) {}
}

void filho_c(void)
{
    for (int i=0; i<1000; i++) {}
    so_escreve('C');
    so_mata_proc(0);
    while (1) {}
}

void init(void)
{
    int b;
    int c;
    b = so_cria_processo((int)filho_b);
    c = so_cria_processo((int)filho_c);
    so_espera_proc(b);
    so_espera_proc(c);
    so_escreve('F');
    so_mata_proc(0);
    while (1) {}
}
