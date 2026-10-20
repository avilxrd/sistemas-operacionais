MANCHA ?= $(HOME)/mancha_so
MCC = $(MANCHA)/compilador_c/bin/mcc
MONTADOR = $(MANCHA)/simulador_completo/bin/montador
SIMULADOR = $(MANCHA)/simulador_completo/bin/simulador
BIOS = $(MANCHA)/bios_mancha/src/bios.asm

.PHONY: all run

all: build/meu_so.mob

build:
	mkdir -p build

build/kernel.asm: src/sistema.c src/kernel.c src/processo.c programas/init.c include/processo.h include/init.h include/syscall.h $(MCC) | build
	$(MCC) -Iinclude src/sistema.c -o $@

build/meu_so.mob: build/kernel.asm src/ponte.asm $(BIOS) $(MONTADOR)
	$(MONTADOR) $(BIOS) src/ponte.asm build/kernel.asm -o $@

run: all
	$(SIMULADOR) build/meu_so.mob
