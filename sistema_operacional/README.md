# Sistema operacional para o Mancha

## Compilar e executar

```sh
make
make run
```

A base fornecida deve estar em `~/mancha_so`. Para outro local, use
`make MANCHA=/caminho/da/base`. O projeto nao altera essa base.
O compilador `mcc` gera Assembly e o montador junta BIOS, ponte e kernel.

### Init()
O programa atual em `programas/init.c` imprime `>`, espera um caractere,
imprime o caractere e encerra. No simulador, execute `D0`, `C` e depois `Ex`,
cada comando seguido de Enter. `Ex` envia `x` e uma quebra de linha ao console.

## Organizacao

- `include/`: estruturas, constantes e declaracoes das funcoes.
- `src/processo.c`: tabela, contextos, fila de prontos e escalonamento.
- `src/kernel.c`: inicializacao, syscalls, pendencias e excecoes.
- `src/metricas.c`: historico, contadores e tempos.
- `src/ponte.asm`: vetores, portas, entrada e retorno das interrupcoes.
- `src/sistema.c`: reune os fontes em uma unidade de compilacao.
- `programas/init.c`: programa executado pelo usuario.
- `testes/`: programas pequenos e executor sem interface, usando a CPU original.
- `build/`: arquivos gerados e saidas dos testes.

## Funcionamento

Cada processo possui PID, estado, contexto de 16 palavras e pilha exclusiva.
O PID nao e o indice da tabela. Morrer libera a vaga, mas preserva o historico.
O contexto inicial usa S=0, I=0 e D=0, o usuario nao pode usar portas nem
instrucoes privilegiadas, mas pode receber interrupcoes.

As cinco syscalls sao `SO_LE`, `SO_ESCREVE`, `SO_CRIA_PROC`, `SO_MATA_PROC`
e `SO_ESPERA_PROC`. A leitura bloqueia quando o buffer esta vazio. 
A BIOS recebe os caracteres, o tratamento periodico de pendencias entrega a 
entrada e desbloqueia o leitor. A saida do console esta sempre pronta,
portanto escrever nao exige bloqueio.

Sem processos prontos, a ponte usa a pilha do nucleo, habilita interrupcoes e
fica ociosa. A pilha dos handlers so e usada enquanto as interrupcoes externas
estao desabilitadas. Uma excecao de CPU encerra o processo culpado, uma excecao
no kernel para a maquina. A morte do init imprime as metricas e para o SO.

## Configuracao

Edite `include/processo.h` e execute `make`:

- `ESCALONADOR`: `ESC_SIMPLES`, `ESC_RR` ou `ESC_PRIORIDADE`.
- `QUANTUM`: quantidade de interrupcoes por fatia, entre 1 e 32.
- `PERIODO_RELOGIO`: 16000 instrucoes na configuracao validada.

O simples mantem o atual ate bloquear ou morrer e escolhe o primeiro pronto
na tabela. RR usa FIFO. Prioridade escolhe o menor valor, preservando
a ordem da fila nos empates. A prioridade inicia em 500 (equivalente a metade)
e usa a formula do enunciado, escalada por 1000. Bloqueio e fim de quantum
atualizam a prioridade. Conta-se uma preempcao a cada fim de quantum, mesmo
se o proprio processo for selecionado novamente.

A configuracao agora fica apenas no header, e a ponte recebe o valor.

## Metricas

- Tempo total: da primeira leitura do relogio na inicializacao das metricas
  ate a captura final, antes da impressao. Inclui trabalho do kernel.
- Tempo ocioso: intervalos em que o escalonador deixou a CPU sem processo.
- Retorno: da primeira entrada em PRONTO ate o encerramento do processo.
- Tempo por estado: acumulado nos pontos em que `registra_estado` e chamada.
  Inclui o trabalho do kernel enquanto o processo permanece naquele estado.
- Resposta: de BLOQUEADO -> PRONTO ate a proxima entrada em EXECUTANDO.
  A criacao e a preempcao nao sao desbloqueios. A media inteira considera
  apenas respostas concluidas; sem amostras aparece `n/a`.
- Processos vivos na morte do init aparecem como ativos, com retorno pendente.
- Interrupcoes: quantidade de atendimentos de cada vetor (0 a 15), inclusive
  o boot. Varias teclas podem produzir um unico atendimento de console.

O hardware guarda apenas um bit de interrupcao pendente.

Limites: 8 processos simultaneos, 64 criacoes por execucao e pilha de 128
palavras por processo. O historico cheio faz a criacao retornar -1. Tempos
cabem ate 327679999 instrucoes. Contadores simples cabem ate 32767 eventos.
O codigo nao implementa isolamento de memoria nem deteccao de ciclos de espera.

## Testes e resultados

```sh
make test
```

Sao 48 combinacoes: oito programas, tres politicas e quantum 1 ou 3.
As saidas completas ficam em `build/testes/*.txt` e o resumo em
`build/testes/resumo.tsv`. Esses arquivos evitam perder linhas por causa do
tamanho do console. `RELATORIO.md` apresenta a comparacao das execucoes.
