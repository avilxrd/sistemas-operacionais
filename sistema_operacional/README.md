# Sistema operacional para o Mancha

### Compilar e executar

```sh
make
make run
```

A base deve estar em ~/mancha_so.

### Organizacao

- `include/processo.h`: estrutura, estados e declaracao de cria_processo.
- `include/init.h`: declaracao de init.
- `src/processo.c`: tabela, identificadores e criacao inicial de processos.
- `src/kernel.c`: inicializacao e preparacao do contexto do init.
- `src/ponte.asm`: entrada pela BIOS e transferencia para o contexto preparado.
- `programas/init.c`: pilha e programa de usuario que conta em laco.
- `src/sistema.c`: reune os fontes em uma unidade de compilacao.
- `build/`: assembly e objeto gerados

### Estado atual

O kernel prepara um unico processo init em modo usuario, que conta
continuamente. O console fica vazio. Chamadas de sistema e escalonamento
ainda nao estao implementados.
