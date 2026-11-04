# AGENTS.md — cpu-renderer-cpp

Diretrizes para agentes que trabalham neste projeto de Computação Gráfica Básica (UFRPE/DC). Leia este arquivo antes de implementar qualquer issue.

## Missão

Manter e evoluir um pipeline gráfico implementado manualmente em C++ (CPU), sem motor gráfico: malha BYU → câmera → projeção → normais → scan line com z-buffer → interpolação perspectiva-correta → iluminação/tonalização de Phong → Canvas.

A 1ª VA entregou carregamento de malha, câmera, projeção e rasterização, e permanece como base do pipeline. A 2ª VA está concluída: acrescenta normais de face e de vértice, z-buffer com `invZ`, interpolação perspectiva-correta, iluminação e tonalização de Phong por pixel e a aplicação completa (CLI, recarga, modos de depuração e dump BMP). O comportamento atual é a fonte de verdade; não reescreva o projeto para “melhorar” o que já está estável sem uma issue que peça isso.

## Leia antes de codar

| Documento | Quando ler |
|---|---|
| [README.md](README.md) | Sempre: comandos, controles, configuração e visão do pipeline |
| [CONTEXT.md](CONTEXT.md) | Sempre: vocabulário canônico; use estes termos em código, docs e relatórios |
| [docs/especificacao-1va.md](docs/especificacao-1va.md) e [docs/requisitos-1va.md](docs/requisitos-1va.md) | Ao mexer em malha, câmera, projeção ou scan line; a 1ª VA é a base |
| [docs/especificacao-2va.md](docs/especificacao-2va.md) e [docs/requisitos-2va.md](docs/requisitos-2va.md) | Ao mexer em iluminação, normais, z-buffer, interpolação ou recarga; ao verificar critérios da 2ª VA |
| [docs/adr/](docs/adr/) | Antes de contrariar uma decisão arquitetural; proponha uma ADR nova |
| Issue atual | Sempre: escopo, critérios de aceitação, dependências e referências |

ADRs versionadas: [0001](docs/adr/0001-contrato-pixel-sdl.md), [0002](docs/adr/0002-fronteira-carregamento-byu.md), [0003](docs/adr/0003-canvas-dual.md), [0004](docs/adr/0004-convencoes-coordenadas.md), [0005](docs/adr/0005-convencoes-scan-line.md), [0006](docs/adr/0006-camera-unica-compartilhada.md), [0007](docs/adr/0007-modelo-iluminacao-phong.md), [0008](docs/adr/0008-normais-face-vertice-two-sided.md), [0009](docs/adr/0009-z-buffer-interpolacao-perspectiva.md) e [0010](docs/adr/0010-camera-iluminacao-arquivos-separados.md).

O PDF original da disciplina não é versionado. As transcrições em `docs/` são a referência local.

## Restrições inegociáveis

- C++17, biblioteca padrão. SDL2 somente para janela e desenho de pixel (`SDL_RenderDrawPoint`).
- Proibido: texturas, `SDL_RenderDrawLine`, `SDL_RenderFillRect`, primitivas de triângulo, `SDL_RenderCopy`, OpenGL, Vulkan, motores gráficos ou bibliotecas de geometria.
- Toda a matemática de câmera, projeção, normais, interpolação, z-buffer, iluminação e rasterização é manual.
- O contrato de desenho é `Canvas`, com `clear(color)`, `drawPixel(x, y, color)`, `width()` e `height()`; `MemoryCanvas` e `WindowSDL` são as implementações.
- Não altere a semântica de `RenderMode::Points`/`Wireframe`: eles são depuração sem iluminação e sem profundidade.
- Não implemente flat shading nem Gouraud shading: o escopo da 2ª VA é somente Phong shading (ADR-0007).
- Não adicione clipping de plano próximo, backface culling, múltiplas luzes, sombras, texturas ou transparência sem uma issue específica.

## Mapa dos módulos

| Módulo | Responsabilidade |
|---|---|
| `src/math/Vec3` | Vetores 3D, produto escalar/vetorial, normalização, escala |
| `src/math/Color` | Cor RGB em `[0,255]`, soma, escala, clamp e empacotamento `0xRRGGBB` |
| `src/mesh/Mesh` | Leitura e armazenamento BYU; índices 1-based → 0-based |
| `src/mesh/Normals` | Normais de face e de vértice |
| `src/pipeline/Camera` | Base `(U,V,N)`, centro `C`, mundo → vista |
| `src/pipeline/Projection` | Perspectiva, normalização, tela e `invZ` |
| `src/pipeline/DepthBuffer` | Buffer de profundidade com `invZ` |
| `src/pipeline/Rasterizer` | Scan line de cor sólida e linhas (caminho legado/depuração) |
| `src/pipeline/ShadedRasterizer` | Scan line com atributos, z-buffer e chamada ao Phong |
| `src/pipeline/Renderer` | Integração do pipeline; sobrecarga iluminada e sobrecarga legada |
| `src/shading/Lighting` | `Light`, `Material`, `Lighting` e parser estrito |
| `src/shading/Phong` | Equação de iluminação e casos especiais |
| `src/canvas/*` | `Canvas`, `MemoryCanvas`, `WindowSDL`, `BmpWriter` |
| `src/main.cpp` | CLI, controles, recarga e seleção de malha |

## Convenções que não podem quebrar

### Coordenadas

- Mundo → vista: `((p−C)·U, (p−C)·V, (p−C)·N)`.
- `N` aponta da câmera para a cena; `U = normalize(V × N)`; `z > 0` está à frente.
- Tela: `x = (x_norm+1)·W/2`, `y = (1−y_norm)·H/2`, com `y` crescendo para baixo.
- Não há transformação de modelo nem matrizes 4×4.

### Cor

- Cor RGB em `[0,255]`, empacotada como `0xRRGGBB` (`Color::packed`); a conversão final limita por canal e arredonda, sem correção gamma.
- O fundo é preto; o caminho legado/depuração desenha em branco. O caminho iluminado calcula a cor por pixel.

### Normais e two-sided

- Normal de face: `normalize((b − a) × (c − a))`; faces degeneradas, de normal nula ou não finita não contribuem.
- Normal de vértice: média não ponderada (soma das normais unitárias das faces incidentes), seguida de normalização; vértice sem face incidente fica com normal nula.
- Após interpolar, renormalize a normal; se for nula ou não finita, use a normal da face; se essa também for nula, a iluminação retorna somente a parcela ambiente.
- Two-sided: inverter `N` se `N·L < 0` e `V·N < 0`; se ainda `N·L < 0`, desconsiderar difusa e especular; se `R·V < 0`, desconsiderar somente a especular.

### Profundidade e rasterização

- Armazene `invZ = 1/z`; maior `invZ` vence. Empate mantém o primeiro fragmento.
- Cobertura por scan line: ordenar por `y`, interseções com arestas não horizontais, preencher o intervalo.
- Atributos (posição de mundo e normal) são interpolados com pesos baricêntricos multiplicados por `invZ` e divididos pelo `invZ` final. **Nunca** interpole `z` linearmente em tela.
- Não ordene triângulos por baricentro; o z-buffer resolve visibilidade.

### Iluminação de Phong

- `L = normalize(Pl − P)`, `V = normalize(C − P)`, `R = 2(N·L)N − L`.
- `V` da iluminação **não** é o vetor `V` da base da câmera.
- `Ia = Ka·Iamb`; `Id = max(N·L,0)·Kd∗Od∗Il`; `Is = max(R·V,0)^eta·Ks·Il`.
- `Iamb`/`Il` em `[0,255]`; `Ka/Ks/Kd/Od` em `[0,1]`; `eta >= 1`.
- Especular não é tingido por `Od`.
- Clamp final por canal em `[0,255]`, com arredondamento; sem gamma.

### Configuração e recarga

- Câmera: `camera/camera.txt`. Iluminação: `lighting/lighting.txt`.
- CLI: até três argumentos posicionais, na ordem `[modelo.byu] [camera.txt] [lighting.txt]`; os omitidos ao final usam `models/piramide.byu`, `camera/camera.txt` e `lighting/lighting.txt`; um quarto posicional é erro.
- O parser de iluminação exige todos os campos, sem chaves duplicadas e sem tokens extras.
- `lighting/lighting-pdf.txt` preserva o exemplo exato do PDF; não o use como padrão sem justificar.
- `R` recarrega câmera **e** iluminação; cada carga é atômica e erro preserva o último valor válido daquele arquivo.
- `1`–`6` troca apenas a malha; câmera e iluminação são compartilhadas.
- `--dump` e `--smoke` usam o caminho Phong; `--dump` grava BMP 24-bit sem abrir janela.

## Fluxo de trabalho por issue

Trabalhe em **uma issue por vez**; não misture escopos de issues diferentes no mesmo commit/PR, salvo quando a issue atual declarar a dependência e o responsável autorizar o trabalho encadeado.

1. Leia a issue inteira e os documentos referenciados. Se houver ambiguidade real, pergunte ao responsável; não invente requisito.
2. Rode `make test` antes de mexer, para conhecer a linha de base.
3. Implemente a menor mudança que satisfaça os critérios de aceitação.
4. Adicione/atualize testes automatizados junto com o código. Prefira testes headless com `MemoryCanvas`.
5. Rode `cmake --build build` e `ctest --test-dir build --output-on-failure`; corrija warnings.
6. Faça a verificação manual pedida pela issue (quando houver): `make dev`, `--dump`, os 6 modelos, recarga com `R`.
7. Atualize `CONTEXT.md` e/ou uma ADR se a mudança criar/alterar um termo ou decisão.
8. Reporte o resultado no formato abaixo. **Não** faça commit, push, escrita via `gh`, fechamento de issues/PR ou merge sem autorização explícita do responsável; quando autorizado, registre as operações de git/GitHub no relatório.

A criação de milestones/issues de uma nova VA é uma ação de planejamento previamente autorizada pelo responsável. Fora isso, qualquer operação de git/GitHub exige autorização explícita.

## Definition of Done

- [ ] `cmake --build build` sem warnings.
- [ ] `ctest --test-dir build --output-on-failure` verde (32/32 no estado atual).
- [ ] `make smoke` exit 0, quando o binário for afetado.
- [ ] Critérios de aceitação da issue atendidos.
- [ ] Testes novos cobrindo o comportamento adicionado.
- [ ] `CONTEXT.md`/ADR atualizados quando aplicável.
- [ ] Operações de git/GitHub realizadas somente com autorização explícita e registradas no relatório.
- [ ] Relatório final com arquivos, comandos, resultados e comentários de código adicionados.

## Comandos

```sh
make build
make dev
make run ARGS="models/maca.byu camera/camera.txt lighting/lighting.txt"
make smoke
make test
make clean

cmake -S . -B build
cmake --build build
cmake --build build --target check
ctest --test-dir build --output-on-failure
ctest --test-dir build -N

SDL_VIDEODRIVER=dummy ./build/cpu-renderer --smoke
./build/cpu-renderer --info models/vaso.byu
./build/cpu-renderer --dump saida.bmp models/vaso.byu camera/camera.txt lighting/lighting.txt
```

Verificação manual: `make dev`; as teclas `1`–`6` percorrem `triangulo`, `piramide`, `maca`, `maca2`, `vaso` e `calice2`; edite `camera/camera.txt` e/ou `lighting/lighting.txt` e pressione `R` para recarregar ambos.

## Armadilhas comuns

| Armadilha | Consequência | Ação correta |
|---|---|---|
| Interpolar `z` linearmente em tela | Cores/profundidade distorcidas | Interpolar `1/z` e ponderar atributos por `invZ` |
| Usar o `V` da câmera como direção de vista | Iluminação errada | `V = normalize(C − P)` |
| Tingir o especular com `Od` | Destaque colorido indevido | Especular usa `Ks` e `Il`, não `Od` |
| Assumir normais no BYU | Normais ausentes/zeradas | Calcular com `computeMeshNormals` |
| Não normalizar a normal interpolada | Intensidade errada | Normalizar após interpolar; fallback para normal da face |
| Ignorar faces com winding invertido | Partes pretas em malhas abertas | Iluminação two-sided |
| Testar profundidade com `z` menor | Lógica invertida | `invZ` maior vence |
| Ordenar triângulos por baricentro | Visibilidade incorreta | Deixar o z-buffer decidir |
| Usar primitivas SDL | Viola o enunciado | Somente `drawPixel`/`SDL_RenderDrawPoint` |
| Recarregar só a câmera com `R` | Iluminação antiga | Recarregar câmera e iluminação |
| Trocar câmera/iluminação ao trocar malha | Quebra o contrato compartilhado | `1`–`6` só troca a malha |
| Adicionar flat/Gouraud | Fora do escopo da 2ª VA | Manter somente Phong shading |
| Commitar/pushar/gh write | Quebra o fluxo manual | Reportar e aguardar o responsável |

## Formato do relatório final

Ao concluir uma issue, reporte:

1. **Issue**: número e título.
2. **Arquivos criados/alterados**: lista com caminhos.
3. **Comandos executados**: build, testes, smoke/dump e verificações manuais.
4. **Resultado**: saída relevante, contagem de testes, exit codes.
5. **Critérios de aceitação**: um a um, com evidência.
6. **Comentários no código**: cada comentário adicionado e o motivo (a política é evitar comentários; nomes claros explicam a intenção).
7. **Riscos/limitações**: o que ficou fora do escopo ou merece atenção.
8. **Confirmação**: operações de git/GitHub realizadas conforme autorização (ou nenhuma, se não autorizado).
