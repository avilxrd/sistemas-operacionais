.equ SO_ESCREVE = 1

; ponto de entrada
_kernel_inicio:
        ld      r0, trata_escreve
        st      r0, (tabela_syscalls+2)

        call    _f_kernel_inicio

        cmp     r0, -1
        jmpc    eq, falha_inicio

        ld      sp, r0              ;aponta o sp para o contexto
        rete

falha_inicio:
        halt

; int so_escreve(int caractere)
; Esta parte executa em modo usuario.
_f_so_escreve:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_ESCREVE
        trap    7

        ld      sp, bp
        pop     bp
        ret

; Esta parte executa em modo supervisor.
trata_escreve:
        ld      r0, r1
        call    bios_putc
        ld      r0, 0
        ret