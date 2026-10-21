#ifndef SYSCALL_H
#define SYSCALL_H

#define SO_ESCREVE   1
#define SO_CRIA_PROC 2
#define SO_MATA_PROC 3

int so_escreve(int caractere);


//arg: endereco da funcao do novo processo
//ret: pid do processo criado | -1
int so_cria_processo(int endereco_inicio);


//arg: pid do processo a ser encerrado | 0: próprio processo
//ret: 
int so_mata_proc(int pid);


#endif