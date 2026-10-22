#include "init.h"
#include "syscall.h"

void filho(void)
{
    so_escreve('B');
    so_mata_proc(0);
    while (1) {}
}

void init(void)
{
    int pid;
    pid = so_cria_processo((int)filho);
    if (so_espera_proc(pid) != 0) so_escreve('!');
    so_escreve('C');
    so_mata_proc(0);
    while (1) {}
}
