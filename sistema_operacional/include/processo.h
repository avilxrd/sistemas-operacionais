#ifndef PROCESSO_H
#define PROCESSO_H

#define MAX_PROCESSOS 8

#define TAMANHO_PILHA 128

#define CTX_SP 6 
#define CTX_IP 7 
#define CTX_SR 8 

//valores possíveis para o estado do processo
#define LIVRE      0
#define PRONTO     1
#define EXECUTANDO 2
#define BLOQUEADO  3

struct Processo
{
    int pid;          //id do processo
    int estado;       //estado do processo
    int contexto[16]; //16 registradores do mancha
    int pid_esperado; //0: nao espera
};

//a funcao recebe o endereco do codigo que o novo
//processo devera executar
int cria_processo(int endereco_inicio);


//
void salva_contexto(int *quadro);


//
void restaura_contexto(int *quadro);


//
void so_escalona(void);


//
int mata_processo(int pid);


//
int espera_processo(int pid);


#endif
