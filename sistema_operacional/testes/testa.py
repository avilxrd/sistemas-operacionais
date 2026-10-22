"""Compila copias temporarias e executa o simulador original sem interface."""
from pathlib import Path
import argparse
import re
import shutil
import subprocess
import tempfile

raiz = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--mancha', type=Path, default=Path.home()/'mancha_so')
args = parser.parse_args()
mancha = args.mancha.resolve()
saida = raiz/'build/testes'
saida.mkdir(parents=True, exist_ok=True)

def comando(args):
    r = subprocess.run([str(x) for x in args], text=True, capture_output=True)
    if r.returncode:
        raise RuntimeError(r.stdout + r.stderr)
    return r

base = mancha/'simulador_completo/src'
executa = saida/'executa'
comando(['gcc', '-Wall', '-Wextra', '-std=gnu11', '-I'+str(base),
         raiz/'testes/executa.c', *[base/(n+'.c') for n in
         ['cpu','dispositivos','instrucao','objeto','memoria']], '-o', executa])
resultados = []
for politica in ['ESC_SIMPLES', 'ESC_RR', 'ESC_PRIORIDADE']:
    for quantum in [1, 3]:
        for caso in ['leitura', 'espera', 'cpu', 'erros', 'rajada', 'limites', 'protecao', 'medias']:
            nome = f'{caso}-{politica}-q{quantum}'
            with tempfile.TemporaryDirectory(prefix='mancha-teste-') as temp:
                projeto = Path(temp)
                for pasta in ['src', 'include', 'programas']:
                    shutil.copytree(raiz/pasta, projeto/pasta)
                shutil.copyfile(raiz/f'testes/programas/{caso}.c', projeto/'programas/init.c')
                header = projeto/'include/processo.h'
                header.write_text(header.read_text().replace('#define QUANTUM 3', f'#define QUANTUM {quantum}')
                                  .replace('#define ESCALONADOR ESC_PRIORIDADE', f'#define ESCALONADOR {politica}'))
                ponte = projeto/'src/ponte.asm'
                ponte.write_text(ponte.read_text() + '\n' + '\n'.join('.pub '+n for n in ['relogio_le_byte_baixo', '_f_relogio_contador', '_f_tempo_soma', '_g_tempo_total', '_g_ultimo_contador']))
                if caso == 'protecao':
                    ponte.write_text(ponte.read_text() + '\n_f_tenta_halt:\n halt\n ret\n_f_syscall_invalida:\n ld r0, 99\n trap 7\n ret\n_f_trap_livre:\n trap 11\n ret\n')
                asm = projeto/'kernel.asm'
                mob = projeto/'teste.mob'
                comando([mancha/'compilador_c/bin/mcc', '-I'+str(projeto/'include'), projeto/'src/sistema.c', '-o', asm])
                comando([mancha/'simulador_completo/bin/montador', mancha/'bios_mancha/src/bios.asm',
                         projeto/'src/ponte.asm', asm, '-o', mob])
                r = subprocess.run([str(executa), str(mob), '200000' if caso == 'leitura' else '-1'],
                                   text=True, capture_output=True)
                (saida/(nome+'.txt')).write_text(r.stdout+'\n'+r.stderr)
                assert r.returncode == 0, nome+'\n'+r.stderr
                assert int(re.search(r'amostras=(\d+)', r.stderr)[1]) > 0
                assert '!' not in r.stdout, nome+' sinalizou erro'
                assert 'Metricas' in r.stdout, nome+' sem metricas'
                antes = r.stdout.split('Metricas')[0]
                assert {'leitura': '>x', 'espera': 'BC', 'erros': 'K'}.get(caso, 'F') in antes, nome
                total, ocioso = map(int, re.search(r'Tempo total: (\d+) \| ocioso: (\d+)', r.stdout).groups())
                assert 0 <= ocioso <= total
                if caso == 'leitura':
                    assert ocioso > 0
                    assert 'Entradas P/E/B: 2/2/1' in r.stdout
                    assert 'respostas concluidas: 1' in r.stdout
                    assert '8:1' in r.stdout
                for retorno, tempos in re.findall(r'encerrado \| retorno: (\d+).*?tempos P/E/B: (\d+/\d+/\d+)', r.stdout, re.S):
                    # O retorno deve ser exatamente a soma dos tempos nos tres estados.
                    assert sum(map(int,tempos.split('/'))) == int(retorno), nome
                preemps = int(re.search(r'Processos criados: \d+ \| preempcoes: (\d+)', r.stdout)[1])
                if caso == 'limites': assert 'Processos criados: 64' in r.stdout
                if caso == 'protecao':
                    assert '2:1' in r.stdout and '11:1' in r.stdout
                if politica == 'ESC_SIMPLES': assert preemps == 0
                if caso == 'cpu' and politica != 'ESC_SIMPLES': assert preemps > 0
                resultados.append((nome, total, ocioso, preemps))
                print('PASS', nome, flush=True)
(saida/'resumo.tsv').write_text('caso\ttotal\tocioso\tpreempcoes\n'+''.join('\t'.join(map(str,r))+'\n' for r in resultados))
print(f'{len(resultados)} testes passaram. Saidas em {saida}')
