#include "init.h"
#include "syscall.h"
#include "metricas.h"

/* Operacoes aritmeticas puras tambem podem ser exercitadas pelo usuario. */
void init(void)
{
    struct Tempo total;
    struct Tempo media;
    total.blocos = 32767;
    total.unidades = 9999;
    tempo_media(&media, &total, 32767);
    if (media.blocos != 1 || media.unidades != 0) so_escreve('!');
    total.blocos = 12;
    total.unidades = 3456;
    tempo_media(&media, &total, 7);
    if (media.blocos != 1 || media.unidades != 7636) so_escreve('!');
    tempo_media(&media, &total, 0);
    if (media.blocos != 0 || media.unidades != 0) so_escreve('!');
    so_escreve('F');
    so_mata_proc(0);
    while (1) {}
}
