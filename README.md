# cpu-renderer-cpp

Pipeline gráfico implementado manualmente em C++ (CPU), reproduzindo as etapas de uma GPU: leitura de malhas no formato BYU, câmera virtual, transformação mundo→vista, projeção em perspectiva, normalização, conversão para coordenadas de tela e rasterização de triângulos com o algoritmo *scan line*.

Projeto da disciplina **Computação Gráfica Básica (06230)** — UFRPE/DC, 1ª VA.

A única biblioteca externa é a SDL2, usada somente para criação da janela e desenho de pixel (`SDL_RenderDrawPoint`); toda a matemática e a rasterização são manuais.

## Requisitos

- CMake >= 3.16
- Compilador com suporte a C++17
- SDL2 (`libsdl2-dev` no Ubuntu/Debian)

## Build e execução

Atalhos via `Makefile`:

```sh
make build                  # configura e compila
make dev                    # compila e executa (padrão: piramide + camera/camera.txt)
make run ARGS="models/maca.byu"                # malha com a câmera padrão
make smoke                  # verificação headless (exit 0)
make test                   # roda os testes (CTest)
make clean                  # remove build/
```

Equivalentes com CMake puro:

```sh
cmake -S . -B build
cmake --build build
cmake --build build --target run      # também: smoke, check
ctest --test-dir build --output-on-failure
```

Exemplos da aplicação:

```sh
./build/cpu-renderer                  # padrão: models/piramide.byu + camera/camera.txt
./build/cpu-renderer models/maca.byu  # malha com a câmera padrão
./build/cpu-renderer models/triangulo.byu camera/camera.txt   # câmera explícita (exemplo do PDF)
./build/cpu-renderer --fullscreen     # inicia em tela cheia
./build/cpu-renderer --info models/vaso.byu                   # contagens de vértices/triângulos
./build/cpu-renderer --dump saida.bmp models/vaso.byu         # BMP com a câmera padrão
SDL_VIDEODRIVER=dummy ./build/cpu-renderer --smoke
```

Argumentos: `[modelo.byu] [camera.txt]` (opcionais, com os padrões acima) e as flags `--info`, `--dump arquivo.bmp`, `--smoke`, `--test-pattern` e `--fullscreen`.

## Controles

| Tecla | Ação |
|-------|------|
| `1`–`6` | Troca a malha (triangulo, piramide, maca, maca2, vaso, calice2); a câmera é compartilhada |
| `R` | Recarrega o arquivo de câmera corrente (`camera/camera.txt` ou o caminho passado na CLI) |
| `D` | Alterna modo pontos |
| `W` | Alterna modo arame |
| `+` / `-` | Zoom (aumenta/diminui `d`) |
| `F11` | Alterna tela cheia |
| `Esc` / `Q` | Encerra a aplicação |

A janela é redimensionável e o desenho acompanha o tamanho real (maximizar preenche a área toda). Em caso de erro ao recarregar/carregar, a última malha ou câmera válida é mantida e a mensagem vai para o `stderr`.

## Parâmetros da câmera

`camera/camera.txt` é o arquivo único de câmera, compartilhado por todos os modelos (exemplo do PDF). Formato (rótulos, ordem livre, `=` ou `:`, comentários com `#`, maiúsculas ou minúsculas):

```
N = 0 1 -1      # direção da câmera para a cena (é normalizada)
V = 0 -1 -1     # vertical de referência (ortogonalizado contra N)
d = 5           # distância focal (deve ser positiva)
hx = 2          # semi-largura do plano de projeção (positiva)
hy = 2          # semi-altura do plano de projeção (positiva)
C = 0 -500 500  # centro da câmera
```

`U` é calculado (`normalize(V × N)`) e não é lido do arquivo. Câmeras degeneradas (vetor nulo ou `V` paralelo a `N`) e escalares não positivos são rejeitados com mensagem.

| Tecla | Malha |
|-------|-------|
| `1` | `models/triangulo.byu` |
| `2` | `models/piramide.byu` |
| `3` | `models/maca.byu` |
| `4` | `models/maca2.byu` |
| `5` | `models/vaso.byu` |
| `6` | `models/calice2.byu` |

A câmera é a mesma para todos os modelos: `camera/camera.txt` (ou o caminho passado na CLI).

## Como o pipeline funciona

1. **Malha (BYU)**: vértices 3D e triângulos com índices 1-based convertidos para 0-based no carregamento.
2. **Câmera**: base ortonormal `(U, V, N)` e centro `C`; cada vértice vira coordenadas de vista `((p−C)·U, (p−C)·V, (p−C)·N)`.
3. **Projeção**: `xp = d·xv/zv` e `yp = d·yv/zv`; normalização por `hx`/`hy`; conversão para tela `((x+1)·largura/2, (1−y)·altura/2)`.
4. **Visibilidade**: pontos com `zv ≤ 0` estão atrás da câmera e o triângulo inteiro é descartado (sem clipping parcial).
5. **Scan line**: cada triângulo é preenchido por varredura em branco sobre fundo preto; os modos `D`/`W` mostram vértices/arestas para depuração.
6. **Saída**: janela SDL2 ou `--dump` (BMP 24-bit gerado sem janela).

## Testes e verificação

```sh
make test    # 16 suítes: mesh, vec3, camera, projection, rasterizer, renderer, bmp_writer,
             # info_sucesso, info_arquivo_inexistente, smoke, dump_sucesso,
             # dump_modelo_inexistente, camera_padrao, camera_policy,
             # sdl_backend_policy, default_render_mode
make smoke   # executa o pipeline headless e exige exit 0
```

Critérios de aceitação do levantamento (§9) e como verificar:

| Critério | Verificação |
|---|---|
| Arquivos BYU carregados corretamente | `--info`, testes `mesh` (6 modelos) |
| Câmera configurável | `camera/camera.txt` compartilhado + `R`; testes `camera` e `camera_padrao` |
| Vértices projetados corretamente | testes `projection` e `renderer` (valores calculados à mão) |
| Triângulos convertidos para pixels | testes `rasterizer` (contagem exata de pixels) |
| Rasterização preenche na tela | `make dev` |
| Diferentes modelos renderizáveis | teclas `1`–`6` (câmera compartilhada) |

## Estrutura do repositório

```
src/
├── math/       Vec3
├── mesh/       leitura e armazenamento BYU
├── pipeline/   câmera, projeção, rasterização e renderer
└── canvas/     Canvas, janela SDL, canvas em memória e escrita BMP
models/         malhas BYU de exemplo
camera/         camera.txt (arquivo único de câmera, exemplo do PDF)
tests/          testes automatizados (CTest)
```

> `docs/` (levantamento, especificação da 1ª VA e ADRs) e `AGENTS.md` são documentos locais de trabalho, mantidos fora do repositório.
> O levantamento (§5) sugere `src/` plano; o projeto agrupa os mesmos módulos por camada, sem alterar responsabilidades ou interfaces.

## Documentação

- `CONTEXT.md` — glossário do domínio
- `docs/` — levantamento de requisitos, especificação da 1ª VA e ADRs 0001–0006 (local, não versionado)
- `AGENTS.md` — diretrizes para agentes e colaboradores (local, não versionado)
