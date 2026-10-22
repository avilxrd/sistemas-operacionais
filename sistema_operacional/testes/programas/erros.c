#include "init.h"
#include "syscall.h"

void filho(void)
{
    while (1) {}
}

void init(void)
{
    int pid;
    int anterior;
    anterior = 1;
    if (so_espera_proc(1) != -1) so_escreve('!');
    if (so_espera_proc(0) != -1) so_escreve('!');
    if (so_espera_proc(999) != -1) so_escreve('!');
    if (so_mata_proc(999) != -1) so_escreve('!');
    if (so_cria_processo(0) != -1) so_escreve('!');
    for (int i=0; i<10; i++)
    {
        pid = so_cria_processo((int)filho);
        if (pid <= anterior) so_escreve('!');
        if (so_mata_proc(pid) != 0) so_escreve('!');
        if (so_mata_proc(pid) != -1) so_escreve('!');
        anterior = pid;
    }
    so_escreve('K');
    so_mata_proc(0);
    while (1) {}
}
