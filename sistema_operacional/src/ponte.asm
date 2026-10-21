.equ SO_ESCREVE     = 1
.equ SO_CRIA_PROC   = 2
.equ SO_MATA_PROC   = 3
.equ SO_ESPERA_PROC = 4

_kernel_inicio:
        ld      r0, entrada_syscall
        st      r0, (56)

        call    _f_kernel_inicio

        cmp     r0, -1
        jmpc    eq, falha_inicio

        ld      sp, r0
        rete

falha_inicio:
        halt

;int so_escreve(int caractere);
_f_so_escreve:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_ESCREVE
        trap    7

        ld      sp, bp
        pop     bp
        ret

entrada_syscall:
        ld      r0, sp
        push    r0
        call    _f_trata_syscall
        add     sp, 2

        cmp     r0, -1
        jmpc    eq, falha_inicio

        rete

_f_console_escreve:
        push    bp
        ld      bp, sp

        ld      r0, (bp+4)
        call    bios_putc

        ld      sp, bp
        pop     bp
        ret

;int so_cria_processo(int endereco_inicio);
_f_so_cria_processo:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_CRIA_PROC
        trap    7

        ld      sp, bp
        pop     bp
        ret

;int so_mata_proc(int pid);
_f_so_mata_proc:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_MATA_PROC
        trap    7

        ld      sp, bp
        pop     bp
        ret

;
_f_so_espera_proc:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_ESPERA_PROC
        trap    7

        ld      sp, bp
        pop     bp
        ret
