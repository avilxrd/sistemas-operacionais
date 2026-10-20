; ponto de entrada
_kernel_inicio:
        call    _f_kernel_inicio

        cmp     r0, -1
        jmpc    eq, falha_inicio

        ld      sp, r0              ;aponta o sp para o contexto
        rete

falha_inicio:
        halt
