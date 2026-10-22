#include "init.h"
#include "syscall.h"

void init(void)
{
    int caractere;
    so_escreve('>');
    caractere = so_le();
    so_escreve(caractere);
    so_escreve(10);
    so_mata_proc(0);
    while (1) {}
}
