# Relatorio de execucoes

## Ambiente e metodo

Execucoes realizadas em 22/10/2026 com o mcc, montador e nucleo do simulador
Mancha fornecidos em `~/mancha_so`. O executor de testes utiliza os mesmos
fontes de CPU, memoria, dispositivos e objeto do simulador interativo.

Configuracoes: periodo de 16000 instrucoes, quantum 1 e 3, politicas simples,
round-robin e prioridade. Sao oito cenarios para cada combinacao, totalizando
48 execucoes. Para reproduzir: `make test`.

O teste verifica o encerramento, a saida esperada, os erros de syscall,
a igualdade entre retorno e soma dos tempos nos estados e a consistencia
do tempo total com uma contagem independente de passos da CPU. A referencia
usa os mesmos instantes em que o kernel le o byte baixo do relogio, sem
consultar o tempo calculado pelo kernel para determinar o tempo esperado.

## Comparacao da carga de CPU

O init cria dois filhos com cargas diferentes e espera pelos terminos. Um
filho faz 1800 iteracoes com aritmetica, o outro faz 1000 iteracoes simples.
Ha disputa de CPU, mas nenhum intervalo sem processo executavel.

| Politica | Quantum | Tempo total | Tempo ocioso | Preempcoes |
|---|---:|---:|---:|---:|
| ESC_SIMPLES | 1 | 121727 | 0 | 0 |
| ESC_SIMPLES | 3 | 121727 | 0 | 0 |
| ESC_RR | 1 | 129441 | 0 | 7 |
| ESC_RR | 3 | 121074 | 0 | 1 |
| ESC_PRIORIDADE | 1 | 129730 | 0 | 7 |
| ESC_PRIORIDADE | 3 | 121224 | 0 | 1 |

Com quantum 1, RR e prioridade fazem mais preempcoes e gastam mais instrucoes
com troca de contexto e atualizacao das metricas. Quantum 3 reduz esse custo.
Na politica simples, mudar o quantum nao altera a execucao, pois nao ha
preempcao por tempo. Estes resultados descrevem esta carga; nao demonstram
que uma politica seja melhor em todos os programas. Prioridade tambem faz
uma busca entre os prontos, diferente da retirada do primeiro elemento em RR.

## Leitura e ociosidade

O init escreve `>`, bloqueia em `so_le`, recebe `x` no passo 200000 e encerra.

| Politica | Quantum | Tempo total | Tempo ocioso | Preempcoes |
|---|---:|---:|---:|---:|
| ESC_SIMPLES | 1 | 216795 | 203340 | 0 |
| ESC_SIMPLES | 3 | 216795 | 203340 | 0 |
| ESC_RR | 1 | 216756 | 203340 | 0 |
| ESC_RR | 3 | 216756 | 203340 | 0 |
| ESC_PRIORIDADE | 1 | 216768 | 203340 | 0 |
| ESC_PRIORIDADE | 3 | 216768 | 203340 | 0 |

Em todas as configuracoes, houve uma entrada em BLOQUEADO, duas em PRONTO,
duas em EXECUTANDO e uma resposta concluida. O tempo ocioso foi positivo.
O bloqueio do processo e a entrada/saida da ociosidade sao registrados em
pontos diferentes do kernel; por isso seus tempos nao precisam ser iguais.

## Outros cenarios

- Espera: o filho imprime B, morre e desbloqueia o init, que imprime C.
- Erros: espera por si mesmo ou PID inexistente, morte invalida, endereco zero
  e dez reutilizacoes de vaga com PIDs sempre novos.
- Rajada: 200 escritas consecutivas, exercitando syscalls perto das voltas do relogio.
- Limites: lotacao das oito vagas, liberacao e preenchimento do historico de
  64 criacoes; a tentativa seguinte retorna -1 sem corromper os registros.
- Protecao: syscall desconhecida e trap reservado retornam -1; tentativa de
  halt em usuario gera a interrupcao 2 e encerra somente o filho.
- Medias: divisao dos tempos em blocos e unidades, inclusive valores grandes
  e ausencia de amostras, sem depender de inteiros maiores que 16 bits.

## Resultado e limites

As 48 execucoes passaram. Todas as amostras verificadas coincidiram com a
referencia independente (erro maximo zero). O retorno de cada processo
encerrado correspondeu a soma dos tempos pronto, executando e bloqueado.

Esses testes cobrem os cenarios descritos, nao todas as intercalacoes possiveis.
O hardware nao informa quantas voltas ocorreram quando varias interrupcoes
ficam acumuladas; handlers longos ou um periodo menor precisam de nova
validacao. Os limites numericos e de memoria estao documentados no README.
A instrumentacao tambem consome instrucoes, portanto os resultados descrevem
o SO com as metricas habilitadas. As saidas completas ficam em `build/testes/`.
