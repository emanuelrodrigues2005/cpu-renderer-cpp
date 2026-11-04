# cpu-renderer-cpp

Pipeline gráfico implementado manualmente em C++ (CPU), sem motor gráfico: malha BYU → câmera → projeção → normais → rasterização *scan line* com `invZ`/z-buffer → interpolação perspectiva-correta → iluminação e tonalização de Phong por pixel → `Canvas`.

Projeto da disciplina **Computação Gráfica Básica (06230)** — UFRPE/DC. A 1ª VA entregou o carregamento de malhas, a câmera, a projeção e a rasterização em branco; essa base continua no pipeline. A 2ª VA é a entrega atual: acrescenta normais de face e de vértice, z-buffer, interpolação perspectiva-correta e iluminação/tonalização de Phong por pixel.

A única biblioteca externa é a SDL2, usada somente para criar a janela e desenhar pixels (`SDL_RenderDrawPoint`); toda a matemática, interpolação, iluminação e rasterização são manuais. O desenho acontece contra o contrato `Canvas`, com `WindowSDL` na janela e `MemoryCanvas` nos testes e no dump.

## Requisitos

- CMake >= 3.16
- Compilador com suporte a C++17
- SDL2 (`libsdl2-dev` no Ubuntu/Debian)

## Build e execução

Atalhos via `Makefile`:

```sh
make build     # configura e compila
make dev       # compila e executa com os caminhos padrão (atalho para make run)
make run ARGS="models/maca.byu camera/camera.txt lighting/lighting.txt"
make smoke     # renderiza um quadro headless e exige exit 0
make test      # executa os 32 testes via CTest
make clean     # remove build/
```

Equivalentes com CMake puro:

```sh
cmake -S . -B build
cmake --build build
cmake --build build --target check   # executa o ctest; também há os alvos run e smoke
ctest --test-dir build --output-on-failure
```

A aplicação recebe até três argumentos posicionais, nesta ordem: `[modelo.byu] [camera.txt] [lighting.txt]`. Os padrões são `models/piramide.byu`, `camera/camera.txt` e `lighting/lighting.txt`; um argumento omitido usa o respectivo padrão. Flags: `--info`, `--dump arquivo.bmp`, `--smoke`, `--test-pattern` e `--fullscreen`.

Exemplos:

```sh
./build/cpu-renderer
./build/cpu-renderer models/maca.byu camera/camera.txt lighting/lighting.txt
./build/cpu-renderer --info models/vaso.byu
./build/cpu-renderer --dump saida.bmp models/vaso.byu camera/camera.txt lighting/lighting.txt
./build/cpu-renderer --test-pattern
./build/cpu-renderer --fullscreen
SDL_VIDEODRIVER=dummy ./build/cpu-renderer --smoke
```

`--dump` grava um BMP 24-bit sem abrir janela e usa o caminho iluminado (Phong). `--smoke` renderiza um quadro headless e retorna sucesso; `make smoke` já define `SDL_VIDEODRIVER=dummy`. `--info` imprime a contagem de vértices e triângulos da malha indicada e encerra. `--test-pattern` desenha o padrão de teste na janela; `--fullscreen` inicia em tela cheia.

## Controles

| Tecla | Ação |
|-------|------|
| `1`–`6` | Troca apenas a malha (triangulo, piramide, maca, maca2, vaso, calice2); câmera e iluminação são compartilhadas e não mudam |
| `R` | Recarrega câmera e iluminação dos caminhos correntes (padrões ou argumentos da CLI) e redesenha |
| `D` | Alterna modo pontos (depuração sem iluminação e sem profundidade) |
| `W` | Alterna modo arame (depuração sem iluminação e sem profundidade) |
| `+` / `-` | Aumenta/diminui a distância focal `d` (zoom) |
| `F11` | Alterna tela cheia |
| `Esc` / `Q` | Encerra a aplicação |

Teclas `1`–`6`, na ordem: `models/triangulo.byu`, `models/piramide.byu`, `models/maca.byu`, `models/maca2.byu`, `models/vaso.byu` e `models/calice2.byu`.

`R` usa os caminhos correntes de câmera e iluminação. Cada arquivo é carregado atomicamente: um erro em uma fonte vai para o `stderr`, preserva o último valor válido daquela fonte e não invalida a outra. Se ao menos uma fonte recarregar com sucesso, a cena é redesenhada. O zoom de `+`/`-` altera `d` em memória; `R` relê o arquivo e restabelece os valores dele.

A janela é redimensionável e o desenho acompanha o tamanho atual. Na troca por `1`–`6`, a carga de malha também é atômica: um arquivo inválido preserva a malha corrente e reporta o erro no `stderr`.

## Formato da câmera

`camera/camera.txt` é o único arquivo de câmera, compartilhado por todas as malhas. Exemplo versionado:

```
N = 0 1 -1      # direção da câmera para a cena (é normalizada)
V = 0 -1 -1     # vertical de referência (ortogonalizado contra N)
d = 5           # distância focal (deve ser positiva)
hx = 2          # semi-largura do plano de projeção (positiva)
hy = 2          # semi-altura do plano de projeção (positiva)
C = 0 -500 500  # centro da câmera
```

Os seis campos são obrigatórios: `N` e `V` (vetores), `C` (ponto), `d`, `hx` e `hy` (escalares). `N` aponta da câmera para a cena; `V` é o vertical de referência, ortogonalizado contra `N`; `U = normalize(V × N)` é calculado e não é lido do arquivo. Rótulos não diferenciam maiúsculas de minúsculas, a ordem é livre, `=` e `:` são aceitos e o restante da linha após `#` é comentário.

A validação rejeita campos desconhecidos, campos ausentes, valores não finitos, `N`/`V` nulos, `V` paralelo a `N` e `d`/`hx`/`hy` não positivos. A carga é atômica: um arquivo inválido preserva a câmera anterior.

## Formato da iluminação

`lighting/lighting.txt` é o arquivo padrão, calibrado para os modelos e a câmera compartilhada:

```
Iamb = 100 100 100
Ka = 0.2
Il = 255 245 220
Pl = 200 -200 400
Kd = 0.5 0.3 0.2
Od = 0.7 0.5 0.8
Ks = 0.5
eta = 1
```

Os oito campos são obrigatórios:

| Campo | Significado | Formato e faixa |
|-------|-------------|-----------------|
| `Iamb` | cor RGB da luz ambiente | três canais em `[0,255]` |
| `Ka` | coeficiente de reflexão ambiental | escalar em `[0,1]` |
| `Il` | cor RGB da fonte de luz | três canais em `[0,255]` |
| `Pl` | posição da fonte de luz em coordenadas de mundo | três valores finitos |
| `Kd` | coeficiente de reflexão difusa | três canais em `[0,1]` |
| `Od` | cor difusa do material | três canais em `[0,1]` |
| `Ks` | coeficiente de reflexão especular | escalar em `[0,1]` |
| `eta` | expoente especular | escalar `>= 1` |

O parser é estrito: exige todos os campos, sem chaves duplicadas, sem tokens extras e sem campos desconhecidos; todos os valores devem ser finitos e estar nas faixas acima. Erros são reportados em português, citando o campo e o caminho do arquivo, e a carga é atômica: um arquivo inválido preserva a última iluminação válida. Como na câmera, rótulos não diferenciam maiúsculas de minúsculas, a ordem é livre, `=` e `:` são aceitos e `#` inicia comentário.

`lighting/lighting-pdf.txt` preserva o exemplo exato do PDF da 2ª VA (luz abaixo da origem); ele é um exemplo de referência e não é o arquivo padrão. A fonte de luz é pontual, definida em coordenadas de mundo, sem atenuação.

## Pipeline

1. **Malha (BYU)** — lê a contagem de vértices e triângulos, as coordenadas dos vértices e os índices 1-based, convertidos para 0-based; conteúdo após o último triângulo é ignorado.
2. **Câmera** — constrói a base ortonormal `(U, V, N)` e o centro `C`; cada vértice vira coordenadas de vista `((p−C)·U, (p−C)·V, (p−C)·N)`, com `N` apontando da câmera para a cena.
3. **Projeção** — `xp = d·xv/zv` e `yp = d·yv/zv`; normalização por `hx`/`hy` e conversão para tela `((x_norm+1)·largura/2, (1−y_norm)·altura/2)`, com `y` crescendo para baixo. Vértices com `zv ≤ 0` são descartados; não há clipping de plano próximo, então um triângulo com qualquer vértice inválido é descartado inteiro.
4. **Normais** — a normal de face é o produto vetorial normalizado de duas arestas; a normal de vértice é a média não ponderada das normais unitárias das faces incidentes, seguida de normalização. Faces degeneradas ou de vetor nulo não contribuem; vértice sem face incidente fica com normal nula; valores não finitos são rejeitados.
5. **Scan line** — os vértices são ordenados por `y`; as interseções com as arestas não horizontais definem o intervalo `[x_min, x_max]` de cada linha, preenchido de `lround(x_min)` a `lround(x_max)` inclusive e limitado ao `Canvas`. Triângulos de área zero são descartados.
6. **Profundidade e interpolação** — cada vértice projeta `invZ = 1/z`; o z-buffer guarda um valor por pixel, o maior `invZ` vence e o empate mantém o primeiro fragmento. Posição de mundo e normal são interpoladas com pesos baricêntricos multiplicados por `invZ` e divididos pelo `invZ` final; `z` nunca é interpolado linearmente em tela. A normal interpolada é renormalizada; se for nula ou não finita, usa-se a normal da face e, se essa também for nula, a iluminação cai no caso de normal nula (apenas ambiente).
7. **Iluminação e tonalização** — a equação de Phong é avaliada em cada pixel visível, usando a posição de mundo e a normal interpoladas; a cor resultante é escrita no `Canvas`.
8. **Saída** — janela SDL2 ou BMP 24-bit (`--dump`). `--dump` e `--smoke` usam o caminho iluminado. Os modos `Points` e `Wireframe` (`D`/`W`) são caminhos de depuração: desenham em branco, sem iluminação e sem profundidade.

Não há transformação de modelo nem matrizes 4×4, clipping contra o plano próximo, backface culling, múltiplas luzes, atenuação, sombras, texturas, transparência, antialiasing, flat shading ou Gouraud shading.

## Iluminação de Phong

Para um ponto `P` com normal unitária `N`, câmera em `C` e fonte pontual em `Pl`:

```text
L = normalize(Pl − P)
V = normalize(C − P)
R = 2(N·L)N − L

Ia = Ka · Iamb
Id = max(N·L, 0) · Kd ∗ Od ∗ Il
Is = max(R·V, 0)^eta · Ks · Il
I  = Ia + Id + Is
```

`∗` indica multiplicação componente a componente. O `V` da iluminação é a direção de vista (do ponto para a câmera) e não é o vetor `V` da base da câmera. O especular usa `Ks` e `Il`, nunca `Od`.

As regras two-sided são:

1. se `N·L < 0` **e** `V·N < 0`, inverter `N`;
2. se ainda `N·L < 0`, desconsiderar as parcelas difusa e especular;
3. se `R·V < 0`, desconsiderar apenas a parcela especular.

A parcela especular só é avaliada quando `N·L > 0`. O resultado é limitado por canal a `[0,255]`, com arredondamento na conversão para pixel e sem correção gamma. Normal nula ou direção de luz/vista degenerada resulta apenas na parcela ambiente.

## Testes e verificação

```sh
make test                      # 32 testes via CTest
make smoke                     # pipeline headless, exige exit 0
./build/cpu-renderer --dump saida.bmp models/vaso.byu camera/camera.txt lighting/lighting.txt
ctest --test-dir build -N      # lista os 32 testes
```

Os **32 testes** estão agrupados por unidade, integração e CLI/configuração:

- **Unidade (12):** `mesh`, `vec3`, `color`, `normals`, `camera`, `projection`, `depth_buffer`, `rasterizer`, `shaded_rasterizer`, `lighting`, `phong`, `bmp_writer`.
- **Integração e renderização (4):** `renderer`, `renderer_phong`, `model_coverage`, `performance`. `model_coverage` renderiza os 6 modelos reais com Phong e exige pelo menos 500 pixels não pretos, 8 cores distintas e variação de cor por modelo; `performance` mede o redesenho headless em 800×600 e exige mediana de até 100 ms.
- **CLI, configuração e políticas (16):** `info_sucesso`, `info_fora_do_diretorio`, `info_arquivo_inexistente`, `smoke`, `dump_sucesso`, `dump_modelo_inexistente`, `camera_padrao`, `sdl_backend_policy`, `camera_policy`, `default_render_mode`, `lighting_policy`, `lighting_padrao`, `lighting_inexistente`, `lighting_invalido`, `smoke_iluminacao_invalida`, `lighting_sucesso`.

## Critérios de aceitação da 2ª VA

A 2ª VA é considerada concluída quando, entre outros critérios detalhados em [docs/requisitos-2va.md](docs/requisitos-2va.md):

- os oito parâmetros de iluminação são carregados com validação estrita, e um arquivo inválido preserva a última iluminação válida;
- as normais de face e de vértice são calculadas para os 6 modelos, sempre finitas, com fallback para a normal da face;
- a equação de Phong (ambiente, difusa e especular), as regras two-sided e o clamp em `[0,255]` são aplicados; o especular não é tingido por `Od`;
- a tonalização é Phong por pixel, com interpolação perspectiva-correta e normal interpolada renormalizada;
- o z-buffer usa `invZ`, o fragmento mais próximo vence independentemente da ordem de desenho e o empate mantém o primeiro;
- o scan line cobre os triângulos e os modos `Points`/`Wireframe` permanecem sem profundidade e sem iluminação;
- os 6 modelos renderizam com pelo menos 500 pixels não pretos, 8 cores distintas e variação de cor;
- `--dump`, `--smoke`, `--info`, `--test-pattern` e `--fullscreen` continuam funcionando, e a SDL2 é usada apenas para janela e pixel;
- o redesenho headless em 800×600 conclui em até 100 ms;
- os 32 testes estão verdes e o build não tem warnings.

## Estrutura do repositório

```
src/
├── math/       Vec3 e Color
├── mesh/       leitura BYU e normais de face/vértice
├── pipeline/   câmera, projeção, profundidade, rasterização e renderer
├── shading/    parâmetros de iluminação e equação de Phong
└── canvas/     Canvas, janela SDL, canvas em memória e escrita BMP
models/         malhas BYU de exemplo
camera/         camera.txt (arquivo único de câmera, exemplo do PDF)
lighting/       lighting.txt padrão e lighting-pdf.txt (exemplo do PDF)
tests/          testes automatizados e scripts de verificação (CTest)
docs/           especificações, requisitos e ADRs
Makefile        atalhos de build, execução, smoke, testes e limpeza
CMakeLists.txt  configuração do build e registro dos testes
```

## Documentação

- [CONTEXT.md](CONTEXT.md) — glossário do domínio.
- [AGENTS.md](AGENTS.md) — diretrizes para agentes e colaboradores (versionado; a verificação final é a issue #30).
- [docs/especificacao-1va.md](docs/especificacao-1va.md) e [docs/requisitos-1va.md](docs/requisitos-1va.md) — enunciado e requisitos da 1ª VA, que permanece como base.
- [docs/especificacao-2va.md](docs/especificacao-2va.md) e [docs/requisitos-2va.md](docs/requisitos-2va.md) — enunciado e requisitos da 2ª VA, com a matriz de rastreabilidade e a evidência de cada critério.
- ADRs versionadas: [0001](docs/adr/0001-contrato-pixel-sdl.md), [0002](docs/adr/0002-fronteira-carregamento-byu.md), [0003](docs/adr/0003-canvas-dual.md), [0004](docs/adr/0004-convencoes-coordenadas.md), [0005](docs/adr/0005-convencoes-scan-line.md), [0006](docs/adr/0006-camera-unica-compartilhada.md), [0007](docs/adr/0007-modelo-iluminacao-phong.md) e [0009](docs/adr/0009-z-buffer-interpolacao-perspectiva.md).
- As ADRs 0008 e 0010 pertencem à issue #29 e ainda não existem. O PDF original da disciplina não é versionado; as transcrições em `docs/` são a referência local.
