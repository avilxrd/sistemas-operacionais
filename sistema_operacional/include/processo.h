#ifndef PROCESSO_H
#define PROCESSO_H

#define MAX_PROCESSOS 8

//valores possíveis para o estado do processo
#define LIVRE      0
#define PRONTO     1
#define EXECUTANDO 2
#define BLOQUEADO  3

struct Processo
{
    int pid;            //id do processo
    int estado;         //estado do processo
    int contexto[16];   //16 registradores do mancha
};

#endif