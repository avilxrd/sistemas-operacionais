#include "init.h"
#include "syscall.h"

void filho(void)
{
    so_le();
    so_mata_proc(0);
    while (1) {}
}

void init(void)
{
    int pids[7];
    int pid;
    int anterior;
    for (int i=0; i<7; i++)
    {
        pids[i] = so_cria_processo((int)filho);
        if (pids[i] < 0) so_escreve('!');
    }
    if (so_cria_processo((int)filho) != -1) so_escreve('!');
    for (int i=0; i<7; i++)
    {
        if (so_mata_proc(pids[i]) != 0) so_escreve('!');
    }
    anterior = 8;
    for (int i=0; i<56; i++)
    {
        pid = so_cria_processo((int)filho);
        if (pid <= anterior) so_escreve('!');
        if (so_mata_proc(pid) != 0) so_escreve('!');
        anterior = pid;
    }
    if (so_cria_processo((int)filho) != -1) so_escreve('!');
    so_escreve('F');
    so_mata_proc(0);
    while (1) {}
}
