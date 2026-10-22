.DEFAULT_GOAL := all
# Caminho da base fornecida; pode ser sobrescrito com make MANCHA=/outro/caminho.
MANCHA ?= $(HOME)/mancha_so
MCC = $(MANCHA)/compilador_c/bin/mcc
MONTADOR = $(MANCHA)/simulador_completo/bin/montador
SIMULADOR = $(MANCHA)/simulador_completo/bin/simulador
BIOS = $(MANCHA)/bios_mancha/src/bios.asm

.DELETE_ON_ERROR:

.PHONY: all run test

test:
	python3 testes/testa.py --mancha $(MANCHA)

all: build/meu_so.mob

build:
	mkdir -p build

# O mcc compila sistema.c; todos os fontes incluidos precisam ser dependencias.
build/kernel.asm: src/sistema.c src/kernel.c src/processo.c programas/init.c include/processo.h include/init.h include/syscall.h src/metricas.c include/metricas.h $(MCC) | build
	$(MCC) -Iinclude src/sistema.c -o $@

# Montagem conjunta resolve simbolos entre BIOS, ponte e codigo gerado.
build/meu_so.mob: build/kernel.asm src/ponte.asm $(BIOS) $(MONTADOR)
	$(MONTADOR) $(BIOS) src/ponte.asm build/kernel.asm -o $@

# Garante a compilacao antes de abrir a interface interativa.
run: all
	$(SIMULADOR) build/meu_so.mob
