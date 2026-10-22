#include "init.h"
#include "syscall.h"

void init(void)
{
    for (int i=0; i<200; i++) so_escreve('.');
    so_escreve('F');
    so_mata_proc(0);
    while (1) {}
}
