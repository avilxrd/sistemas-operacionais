# Sistemas Operacionais
Miguel Avila de Oliveira

## Trabalho 1
Para implementar o código que gera um número aleatório a minha ideia inicial era de usar o relogio do processador para obter uma seed aleatória, porém ele não estava incrementando. 

Para contornar o problema do relogio, implementei um laço de repetição que incrementa o registrador `r4` a cada iteração, e ao enviar algum dado para o processador via simulador o laço é interrompido e o valor atual do registrador `r4` é utilizado como seed para gerar um número aleatório (0 a 99) diferente a cada execução do programa.

Com a seed gerada, é aplicado o algorítmo LGC clássico, já utilizado em várias aplicações.
