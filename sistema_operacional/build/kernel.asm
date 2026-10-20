; ===== código gerado por mcc =====
.text
_f_init:
	push bp
	ld bp, sp
	ld r0, 0
	st r0, (_g_counter_init)
_L5de1c767_5:
	ld r0, 1
	cmp r0, 0
	jmpc eq, _L5de1c767_6
	ld r0, (_g_counter_init)
	push r0
	add r0, 1
	st r0, (_g_counter_init)
	pop r0
	ld r0, (_g_counter_init)
	push r0
	ld r0, 100
	ld r1, r0
	pop r0
	cmp r0, r1
	jmpc eq, _L5de1c767_7
	ld r0, 0
	jmp _L5de1c767_8
_L5de1c767_7:
	ld r0, 1
_L5de1c767_8:
	cmp r0, 0
	jmpc eq, _L5de1c767_9
	ld r0, 0
	st r0, (_g_counter_init)
_L5de1c767_9:
	jmp _L5de1c767_5
_L5de1c767_6:
	ld sp, bp
	pop bp
	ret
_f_cria_processo:
	push bp
	ld bp, sp
	sub sp, 4
	ld r0, 0
	st r0, (bp+-2)
_L5de1c767_23:
	ld r0, (bp+-2)
	push r0
	ld r0, 8
	ld r1, r0
	pop r0
	cmp r0, r1
	jmpc lt, _L5de1c767_26
	ld r0, 0
	jmp _L5de1c767_27
_L5de1c767_26:
	ld r0, 1
_L5de1c767_27:
	cmp r0, 0
	jmpc eq, _L5de1c767_25
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-2)
	mul r0, 36
	pop r1
	add r1, r0
	ld r0, (r1+2)
	push r0
	ld r0, 0
	ld r1, r0
	pop r0
	cmp r0, r1
	jmpc eq, _L5de1c767_28
	ld r0, 0
	jmp _L5de1c767_29
_L5de1c767_28:
	ld r0, 1
_L5de1c767_29:
	cmp r0, 0
	jmpc eq, _L5de1c767_30
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-2)
	mul r0, 36
	pop r1
	add r1, r0
	push r1
	ld r0, (_g_proximo_pid)
	pop r1
	st r0, (r1)
	ld r0, (_g_proximo_pid)
	push r0
	add r0, 1
	st r0, (_g_proximo_pid)
	pop r0
	ld r0, 0
	st r0, (bp+-4)
_L5de1c767_31:
	ld r0, (bp+-4)
	push r0
	ld r0, 16
	ld r1, r0
	pop r0
	cmp r0, r1
	jmpc lt, _L5de1c767_34
	ld r0, 0
	jmp _L5de1c767_35
_L5de1c767_34:
	ld r0, 1
_L5de1c767_35:
	cmp r0, 0
	jmpc eq, _L5de1c767_33
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-2)
	mul r0, 36
	pop r1
	add r1, r0
	ld r0, r1
	add r0, 4
	push r0
	ld r0, (bp+-4)
	mul r0, 2
	pop r1
	add r1, r0
	push r1
	ld r0, 0
	pop r1
	st r0, (r1)
_L5de1c767_32:
	ld r0, (bp+-4)
	push r0
	add r0, 1
	st r0, (bp+-4)
	pop r0
	jmp _L5de1c767_31
_L5de1c767_33:
	ld r0, (bp+-2)
	ld sp, bp
	pop bp
	ret
_L5de1c767_30:
_L5de1c767_24:
	ld r0, (bp+-2)
	push r0
	add r0, 1
	st r0, (bp+-2)
	pop r0
	jmp _L5de1c767_23
_L5de1c767_25:
	ld r0, 1
	xor r0, -1
	add r0, 1
	ld sp, bp
	pop bp
	ret
	ld sp, bp
	pop bp
	ret
_f_kernel_inicio:
	push bp
	ld bp, sp
	sub sp, 4
	ld r0, 1
	xor r0, -1
	add r0, 1
	st r0, (_g_processo_atual)
	ld r0, 1
	st r0, (_g_proximo_pid)
	ld r0, 0
	st r0, (bp+-4)
_L5de1c767_44:
	ld r0, (bp+-4)
	push r0
	ld r0, 8
	ld r1, r0
	pop r0
	cmp r0, r1
	jmpc lt, _L5de1c767_47
	ld r0, 0
	jmp _L5de1c767_48
_L5de1c767_47:
	ld r0, 1
_L5de1c767_48:
	cmp r0, 0
	jmpc eq, _L5de1c767_46
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-4)
	mul r0, 36
	pop r1
	add r1, r0
	push r1
	ld r0, 0
	pop r1
	st r0, (r1)
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-4)
	mul r0, 36
	pop r1
	add r1, r0
	push r1
	ld r0, 0
	pop r1
	st r0, (r1+2)
_L5de1c767_45:
	ld r0, (bp+-4)
	push r0
	add r0, 1
	st r0, (bp+-4)
	pop r0
	jmp _L5de1c767_44
_L5de1c767_46:
	call _f_cria_processo
	st r0, (bp+-2)
	ld r0, (bp+-2)
	push r0
	ld r0, 1
	xor r0, -1
	add r0, 1
	ld r1, r0
	pop r0
	cmp r0, r1
	jmpc eq, _L5de1c767_49
	ld r0, 0
	jmp _L5de1c767_50
_L5de1c767_49:
	ld r0, 1
_L5de1c767_50:
	cmp r0, 0
	jmpc eq, _L5de1c767_51
	ld r0, 1
	xor r0, -1
	add r0, 1
	ld sp, bp
	pop bp
	ret
_L5de1c767_51:
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-2)
	mul r0, 36
	pop r1
	add r1, r0
	ld r0, r1
	add r0, 4
	push r0
	ld r0, 6
	mul r0, 2
	pop r1
	add r1, r0
	push r1
	ld r0, _g_pilha_init
	push r0
	ld r0, 128
	mul r0, 2
	pop r1
	add r1, r0
	ld r0, r1
	pop r1
	st r0, (r1)
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-2)
	mul r0, 36
	pop r1
	add r1, r0
	ld r0, r1
	add r0, 4
	push r0
	ld r0, 7
	mul r0, 2
	pop r1
	add r1, r0
	push r1
	ld r0, _f_init
	pop r1
	st r0, (r1)
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-2)
	mul r0, 36
	pop r1
	add r1, r0
	push r1
	ld r0, 2
	pop r1
	st r0, (r1+2)
	ld r0, (bp+-2)
	st r0, (_g_processo_atual)
	ld r0, _g_tabela_processos
	push r0
	ld r0, (bp+-2)
	mul r0, 36
	pop r1
	add r1, r0
	ld r0, r1
	add r0, 4
	push r0
	ld r0, 0
	mul r0, 2
	pop r1
	add r1, r0
	ld r0, r1
	ld sp, bp
	pop bp
	ret
	ld sp, bp
	pop bp
	ret

.data
_g_tabela_processos:
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
_g_processo_atual:
	.db 0
	.db 0
_g_proximo_pid:
	.db 0
	.db 0
_g_pilha_init:
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
	.db 0
_g_counter_init:
	.db 0
	.db 0

; ----- literais de string -----
