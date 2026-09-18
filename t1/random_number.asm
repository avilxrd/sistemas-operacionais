; valores iniciais para o funcionamento do simulador
.equ    pilha = 0x3F0

        .org    0
        .dw     main, pilha, 0, 0

        .org    0x80

main:
        inb     r0, (0001)

        ldq     r4, 0

; incrementa o registrador r4 ate que seja enviado algum dado
; para a porta 0001 (no simulador com a tecla "e")
espera_tecla:
        addq     r4, 1               ; r4++
        inb     r2, (0001)           ; le se algo foi enviado
        andb    r2, 1                ; 1 indica caractere recebido
        jmpc    z, espera_tecla      ;

; ao detectar tecla enviada, pega o valor do registrador r4
; e utiliza como seed para gerar o numero aleatorio
        ld      r0, r4               ; r0 = seed
        inb     r2, (0001)           ; esvaziar o buffer

; algoritmo LGC em asm
        ld      r1, 25173
        mul     r0, r1               ; r0 = r0 * 25173
        add     r0, 13849            ; r0 = r0 + 13849

; reduz para um valor entre 0 e 100
        push    r0
        ldq     r1, 100
        ld      r2, r0
        div     r2, r1               ; r2 = r0 / 100
        mul     r2, r1               ; r2 = quociente * 100
        pop     r0
        sub     r0, r2               ; r0 = resto

; trata os restos negativos
        cmp     r0, 0
        jmpc    ge, calcula_digitos
        add     r0, 100

calcula_digitos:
; formata os dados em dezena e unidade para a impressao
        ld      r3, r0               ; r3 = valor total
        ldq     r1, 10
        div     r0, r1               ; r0 = dezena
        ld      r2, r0               ; r2 = dezena
        mul     r0, r1
        sub     r3, r0               ; r3 = unidade

; imprime os dados no console do simulador (em ascii)
        ld      r0, r2
        addb    r0, 48               ; dezena em ASCII
        outb    r0, (0001)

        ld      r0, r3
        addb    r0, 48               ; unidade em ASCII
        outb    r0, (0001)

        ldq     r0, 10
        outb    r0, (0001)

        halt