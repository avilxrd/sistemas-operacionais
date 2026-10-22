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

        ld      r0, entrada_irq_1
        st      r0, (8)
        ld      r0, entrada_irq_2
        st      r0, (16)
        ld      r0, entrada_irq_3
        st      r0, (24)
        ld      r0, entrada_irq_4
        st      r0, (32)
        ld      r0, entrada_irq_5
        st      r0, (40)
        ld      r0, entrada_irq_6
        st      r0, (48)
        ld      r0, pilha_sistema
        st      r0, (50)
        ld      r0, entrada_irq_8
        st      r0, (64)
        ld      r0, entrada_irq_9
        st      r0, (72)
        ld      r0, entrada_irq_11
        st      r0, (88)
        ld      r0, pilha_sistema
        st      r0, (90)
        ld      r0, entrada_irq_12
        st      r0, (96)
        ld      r0, pilha_sistema
        st      r0, (98)
        ld      r0, entrada_irq_13
        st      r0, (104)
        ld      r0, pilha_sistema
        st      r0, (106)
        ld      r0, entrada_irq_14
        st      r0, (112)
        ld      r0, pilha_sistema
        st      r0, (114)
        ld      r0, entrada_irq_15
        st      r0, (120)
        ld      r0, pilha_sistema
        st      r0, (122)

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
        push    r0
        ld      r0, -1
        push    r0
        call    _f_atualiza_tempo
        add     sp, 2
        pop     r0
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

;void console_inteiro(int valor);
_f_console_inteiro:
        push    bp
        ld      bp, sp

        ld      r0, (bp+4)
        call    bios_imprime_dec

        ld      sp, bp
        pop     bp
        ret

; Descricao: le os dois bytes do contador, repetindo se o alto mudou.
; Arg: nenhum. Retorno: contador em r0. Uso interno em supervisor.
_f_relogio_contador:
        push    bp
        ld      bp, sp

relogio_repete_leitura:
        inb     r0, (0x20)
        and     r0, 255
relogio_le_byte_baixo:
        inb     r1, (0x21)
        and     r1, 255
        inb     r2, (0x20)
        and     r2, 255

        cmp     r0, r2
        jmpc    eq, relogio_leitura_valida
        jmp     relogio_repete_leitura

relogio_leitura_valida:
        shl     r0, 8
        or      r0, r1

        ld      sp, bp
        pop     bp
        ret
; Descricao: configura e zera o temporizador durante a inicializacao.
; Arg: periodo em bp+4. Retorno: nenhum.
_f_relogio_inicia:
        push    bp
        ld      bp, sp
        ld      r0, (bp+4)
        ld      r1, r0
        shr     r1, 8
        outb    r1, (0x22)
        outb    r0, (0x23)
        ld      r0, 0
        outb    r0, (0x20)
        outb    r0, (0x21)
        ld      sp, bp
        pop     bp
        ret

; Entrada do vetor 1. O contexto original continua no topo da pilha.
entrada_irq_1:
        ld      r1, sp
        ld      r0, 1
        push    r0
        push    r1
        call    _f_trata_excecao
        add     sp, 4
        jmp     retorno_kernel

; Entrada do vetor 2. O contexto original continua no topo da pilha.
entrada_irq_2:
        ld      r1, sp
        ld      r0, 2
        push    r0
        push    r1
        call    _f_trata_excecao
        add     sp, 4
        jmp     retorno_kernel

; Entrada do vetor 3. O contexto original continua no topo da pilha.
entrada_irq_3:
        ld      r1, sp
        ld      r0, 3
        push    r0
        push    r1
        call    _f_trata_excecao
        add     sp, 4
        jmp     retorno_kernel

; Entrada do vetor 4. O contexto original continua no topo da pilha.
entrada_irq_4:
        ld      r1, sp
        ld      r0, 4
        push    r0
        push    r1
        call    _f_trata_excecao
        add     sp, 4
        jmp     retorno_kernel

; Entrada do vetor 5. O contexto original continua no topo da pilha.
entrada_irq_5:
        ld      r1, sp
        ld      r0, 5
        push    r0
        push    r1
        call    _f_trata_excecao
        add     sp, 4
        jmp     retorno_kernel

; Entrada do vetor 6. O contexto original continua no topo da pilha.
entrada_irq_6:
        ld      r0, 6
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        ld      r0, -1
        st      r0, (sp)
        ld      r0, 0
        jmp     retorno_kernel

; Entrada do vetor 8. O contexto original continua no topo da pilha.
entrada_irq_8:
        ld      r0, 8
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        jmp     _bios_trata_console

; Entrada do vetor 9. O contexto original continua no topo da pilha.
entrada_irq_9:
        ld      r0, 9
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        jmp     _bios_trata_disco

; Entrada do vetor 11. O contexto original continua no topo da pilha.
entrada_irq_11:
        ld      r0, 11
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        ld      r0, -1
        st      r0, (sp)
        ld      r0, 0
        jmp     retorno_kernel

; Entrada do vetor 12. O contexto original continua no topo da pilha.
entrada_irq_12:
        ld      r0, 12
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        ld      r0, -1
        st      r0, (sp)
        ld      r0, 0
        jmp     retorno_kernel

; Entrada do vetor 13. O contexto original continua no topo da pilha.
entrada_irq_13:
        ld      r0, 13
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        ld      r0, -1
        st      r0, (sp)
        ld      r0, 0
        jmp     retorno_kernel

; Entrada do vetor 14. O contexto original continua no topo da pilha.
entrada_irq_14:
        ld      r0, 14
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        ld      r0, -1
        st      r0, (sp)
        ld      r0, 0
        jmp     retorno_kernel

; Entrada do vetor 15. O contexto original continua no topo da pilha.
entrada_irq_15:
        ld      r0, 15
        push    r0
        call    _f_registra_interrupcao
        add     sp, 2
        ld      r0, -1
        st      r0, (sp)
        ld      r0, 0
        jmp     retorno_kernel
