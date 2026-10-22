#include "init.h"
#include "syscall.h"
void tenta_halt(void);
int syscall_invalida(void);
int trap_livre(void);

void filho(void)
{
    tenta_halt();
    so_escreve('!');
    while (1) {}
}

void init(void)
{
    int pid;
    if (syscall_invalida() != -1) so_escreve('!');
    if (trap_livre() != -1) so_escreve('!');
    pid = so_cria_processo((int)filho);
    so_espera_proc(pid);
    so_escreve('F');
    so_mata_proc(0);
    while (1) {}
}
