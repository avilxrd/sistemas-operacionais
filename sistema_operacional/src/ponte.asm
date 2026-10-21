; syscall: r0=numero, r1=argumento; trap 7 entra no supervisor.
; Os numeros devem coincidir com include/syscall.h.
.equ SO_ESCREVE     = 1
.equ SO_CRIA_PROC   = 2
.equ SO_MATA_PROC   = 3
.equ SO_ESPERA_PROC = 4
.equ SO_LE          = 5

; Entrada chamada pela BIOS, com interrupcoes externas desabilitadas.
_kernel_inicio:
        ld      r0, entrada_syscall
        st      r0, (56)

        ld      r0, entrada_relogio
        st      r0, (retomada_escalonador)

        ld      r0, 4000
        ld      r1, r0
        shr     r1, 8
        outb    r1, (0x22)
        outb    r0, (0x23)

        ld      r0, 5
        outb    r0, (0x30)

        call    _f_kernel_inicio

        cmp     r0, -1
        jmpc    eq, falha_inicio

        ld      sp, r0      ; aponta para o contexto inicial montado em C
        rete                ; carrega registradores e entra no processo usuario


; Destino em caso de falha no boot e pedido de parada pelo kernel.
falha_inicio:
        halt


; Pede a escrita ao SO e recebe o resultado em r0.
_f_so_escreve:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_ESCREVE
        trap    7

        ld      sp, bp
        pop     bp
        ret


; Captura SP antes de empilhar o argumento para trata_syscall(quadro).
entrada_syscall:
        ld      r0, sp
        push    r0
        call    _f_trata_syscall
        add     sp, 2
        jmp     retorno_kernel

entrada_relogio:
        ld      r0, sp
        push    r0
        call    _f_trata_relogio
        add     sp, 2
        jmp     retorno_kernel

retorno_kernel:
        cmp     r0, -1
        jmpc    eq, falha_inicio

        cmp     r0, 1
        jmpc    eq, entra_ocioso

        rete

entra_ocioso:
        ; A pilha de interrupcao sera reutilizada
        ; O kernel ocioso usa a pilha separada do nucleo
        ld      sp, pilha_nucleo
        ei

laco_ocioso:
        jmp     laco_ocioso


; Adapta argumento C a bios_putc(r0).
; Exige supervisor ou permissao de E/S. Nao e uma syscall de usuario.
_f_console_escreve:
        push    bp
        ld      bp, sp

        ld      r0, (bp+4)
        call    bios_putc

        ld      sp, bp
        pop     bp
        ret


; Recebe endereco inicial e devolve PID ou -1.
_f_so_cria_processo:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_CRIA_PROC
        trap    7

        ld      sp, bp
        pop     bp
        ret


; PID 0 solicita o proprio encerramento.
; O retorno so e executado se o chamador continuar vivo.
_f_so_mata_proc:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_MATA_PROC
        trap    7

        ld      sp, bp
        pop     bp
        ret


; O processo pode bloquear no trap.
; Quando voltar a CPU continua depois do trap com o resultado em r0.
_f_so_espera_proc:
        push    bp
        ld      bp, sp

        ld      r1, (bp+4)
        ld      r0, SO_ESPERA_PROC
        trap    7

        ld      sp, bp
        pop     bp
        ret


;
_f_so_le:
        push    bp
        ld      bp, sp

        ld      r0, SO_LE
        trap    7

        ld      sp, bp
        pop     bp
        ret


;
_f_console_disponivel:
        push    bp
        ld      bp, sp

        call    bios_console_disponivel

        ld      sp, bp
        pop     bp
        ret


;
_f_console_le:
        push    bp
        ld      bp, sp

        call    bios_console_le

        ld      sp, bp
        pop     bp
        ret