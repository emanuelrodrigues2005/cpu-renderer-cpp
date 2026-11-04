Tracker: GitHub issue #27; este documento é o índice ordenado; não há duplicação em `tasks/todo.md`.

# Plano de implementação — Issue #27: cobertura headless e critérios de aceitação da 2ª VA

## 1. Visão geral e objetivo

A #27 fecha a evidência automatizada da 2ª VA sem alterar o pipeline: adiciona cobertura dos **6 modelos reais** em 800×600, uma prova de integração de **two-sided** no `Renderer` e um teste de **desempenho RNF06** com build otimizada, além de atualizar `CMakeLists.txt`, o target `check` e os checkboxes/tabela de cobertura de `docs/requisitos-2va.md`.

A linha de base é **30 testes verdes** (`ctest`), `make smoke` exit 0, build sem warnings e working tree limpo em `test/m27` (a partir de `dev` em `d43541b`). A contagem final esperada é **32 testes**: `30 + model_coverage + performance`; `renderer_phong` é ampliado, sem novo CTest.

## 2. Não-objetivos

- **Nenhuma alteração em `src/`**: a issue é de testes/cobertura; se um teste revelar defeito real, reportar e abrir issue separada, não corrigir aqui.
- Não alterar `camera/camera.txt`, `lighting/lighting.txt`, os modelos, a câmera compartilhada do app nem a semântica de `Points`/`Wireframe`.
- Não reescrever `README.md` (#28), não criar/alterar ADRs nem revisar ADRs (#29).
- Não duplicar testes já existentes de z-buffer, Phong, parser, normais, dump ou CLI; apenas citá-los na matriz de cobertura.
- Não adicionar dependências, primitivas SDL, flags de produção ou mudanças no workflow de CI (o CI já é headless com `SDL_VIDEODRIVER=dummy`).

## 3. Decisões de arquitetura de teste

### 3.1 Cobertura dos 6 modelos: novo `test_model_coverage`

Criar `tests/test_model_coverage.cpp` e o CTest `model_coverage`, em vez de reforçar `test_renderer_phong::modelos_reais_renderizam_com_phong`.

| Aspecto | Decisão |
|---|---|
| Arquivo/teste | Novo `tests/test_model_coverage.cpp`; target `test_model_coverage`; CTest `model_coverage`. |
| Por que não estender `test_renderer_phong` | O teste existente é um smoke 400×300 com `>0` pixels e `>=2` cores; mantê-lo preserva a regressão e evita misturar dois critérios. O novo teste expressa literalmente a aceitação da 2ª VA. |
| Câmera | Carregar `camera/camera.txt` com `loadCameraFromFile`, copiar para uma variável local e ajustar **apenas `d = 20.0F`**; base `(U,V,N)` e `C` permanecem os do arquivo. A câmera compartilhada do app não é tocada. |
| Iluminação | Carregar `lighting/lighting.txt` com `loadLightingFromFile`. |
| Dimensões | `MemoryCanvas{800, 600}` por modelo. |
| Critérios por modelo | `countNonBlack >= 500`; cores distintas não pretas `>= 8`; variação de cor entre pixels: faixa máxima de canal entre pixels não pretos `>= 2`. |
| Evidência | Imprimir por modelo: caminho, pixels não pretos, cores distintas e faixa de canal; comparar com a auditoria. |

Contagens auditadas com `d = 20` (todas passam com margem):

| Modelo | Não pretos | Cores |
|---|---:|---:|
| triangulo | 2.282 | 11 |
| piramide | 7.293 | 53 |
| maca | 140.864 | 266 |
| maca2 | 87.219 | 228 |
| vaso | 216.232 | 2.741 |
| calice2 | 235.757 | 2.174 |

Com a câmera padrão (`d = 5`), `triangulo` tem 146 pixels e `piramide` 463 — abaixo de 500 —, o que justifica a câmera de teste local e documenta por que o `d = 20` não é arbitrário.

### 3.2 Two-sided: integração em `test_renderer_phong`

Adicionar uma função em `tests/test_renderer_phong.cpp` (sem novo executável/CMake), pois o alvo já linka o pipeline completo e é o lugar natural da integração. A cobertura de two-sided **já existe** em `test_renderer_phong::iluminacao_usa_centro_da_camera_em_mundo` (triângulo `+Z`, `C=(0,0,-10)`, luz atrás, `Iamb=0`, exige branco); a nova função é uma **regressão focada** com controle de winding e pixel interior, e deve ser creditada como tal na tabela de cobertura.

Cenário determinístico já validado:

- câmera sintética com `C = (0,0,0)`, olhando `+Z`, `d = 5`, `hx = hy = 2`;
- triângulo em `z = 5`, winding com normal de face `+Z` (aponta para longe da câmera);
- luz `Il = 255 255 255`, `Pl = (0,0,-10)` (atrás da câmera), `Iamb = 0`, `Kd = 1`, `Od = 1`, `Ks = 0`;
- sem two-sided, `N·L < 0` produziria preto; com a regra (a), `N·L < 0` e `V·N < 0`, a normal é invertida e o pixel fica **branco**;
- controle: a mesma geometria com winding invertido (normal `-Z`, voltada para a câmera) também fica branca;
- usar um pixel **interior** do triângulo, não o centro `(400,300)`, que é vértice: o ponto de mundo `(0.5, 0.5, 5)` projeta em `(500,225)`; comparar esse pixel com `phongColor({0.5,0.5,5}, {0,0,-1}, camera.c, lighting)` e exigir branco (ou diferença máxima ≤ 1), além de `countNonBlack > 0`.

Isso prova a integração `computeMeshNormals` → `renderMesh` → `ShadedRasterizer` → `Phong`; não duplica `test_phong`, que testa a equação isolada.

### 3.3 Performance RNF06: `test_performance` otimizado

Criar `tests/test_performance.cpp` e o CTest `performance`.

| Item | Decisão |
|---|---|
| Medição | `renderMesh` + `MemoryCanvas` 800×600, sem `WindowSDL`, sem SDL, sem dump. |
| Câmera/iluminação | Carregar `camera/camera.txt` e `lighting/lighting.txt` (câmera padrão, sem zoom), como no app. |
| Amostragem | 2 redesenhos de aquecimento + 10 medições por modelo; mediana das 10; imprimir mediana, mínimo e máximo. |
| Orçamento | Mediana `<= 100 ms` para cada um dos 6 modelos. |
| Build | Target compilado com `-O3` e `NDEBUG` **quando `CMAKE_BUILD_TYPE` estiver vazio ou for `Debug`**; em Release usa o `-O3 -DNDEBUG` global, sem sobrescrever. Justificativa: `-O0` mede 111,1 ms para `maca` e faria `make test` local falhar; o RNF06 exige Release. O relatório registra máquina (CPU, núcleos, RAM), `g++ --version` e flags. |
| CTest | `RUN_SERIAL TRUE` para não competir com `ctest -j`; `TIMEOUT 120`; `WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}`. |
| Robustez | Mediana + aquecimento + margem de ~4× sobre a auditoria; não relaxar o limite de 100 ms se o CI falhar — investigar medição/build. |

Auditoria Release-like (`-O3 -DNDEBUG`, câmera padrão): triangulo 0,52 ms; piramide 0,51 ms; maca 24,0 ms; maca2 16,7 ms; vaso 17,3 ms; calice2 22,3 ms. Com `d = 20`, máximo 25,3 ms. O teste usa a câmera padrão porque RNF06 não exige zoom e ela é mais fiel ao app. Pré-validar que o `-O3` não gera warnings em `src/`; se gerar, abrir issue e não editar/suprimir `src/`.

### 3.4 `CMakeLists.txt` e target `check`

- Adicionar `test_model_coverage` e `test_performance` com as mesmas fontes de renderização de `test_renderer_phong` (Renderer, Rasterizer, ShadedRasterizer, Projection, Camera, DepthBuffer, Phong, Lighting, Color, Vec3, Mesh, Normals, MemoryCanvas), `-Wall -Wextra -Wpedantic` e `target_include_directories`.
- Registrar `add_test(NAME model_coverage COMMAND test_model_coverage)` e `add_test(NAME performance COMMAND test_performance)`, com `WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}`; `model_coverage` com `TIMEOUT 30`.
- Em `performance`, adicionar `-O3 -DNDEBUG` **se `CMAKE_BUILD_TYPE` estiver vazio ou for `Debug`** (em Release valem os flags globais), `TIMEOUT 120` e `RUN_SERIAL TRUE`.
- Acrescentar `test_model_coverage` e `test_performance` ao `DEPENDS` do target `check`.
- Não refatorar os blocos existentes; manter o estilo explícito atual.

### 3.5 Documentação em `docs/requisitos-2va.md`

- Marcar os **17 critérios** da seção 4 como `[x]` somente com evidência; distinguir na tabela **automatizado**, **estático** e **manual** (R ao vivo, `--test-pattern`, `--fullscreen` e controles interativos são manuais/estáticos; a evidência manual da #26 pode ser citada).
- Adicionar uma subseção de cobertura com tabela `Critério → evidência automatizada → verificação manual`.
- Citar explicitamente os testes da #26 (`lighting_padrao`, `lighting_sucesso`, `lighting_inexistente`, `lighting_invalido`, `smoke_iluminacao_invalida`, `lighting_policy`, `info_fora_do_diretorio`) e os novos (`model_coverage`, `performance`, `renderer_phong` ampliado). Creditar que `renderer_phong::iluminacao_usa_centro_da_camera_em_mundo` já cobre two-sided; a nova função é regressão focada.
- Para RNF04 (apenas SDL para janela/pixel), citar `sdl_backend_policy` como cobertura estática e registrar que a verificação final de AGENTS/documentação é #30.
- Registrar:
  - a câmera de cobertura é cópia local com `d = 20`; a câmera do app não muda;
  - o procedimento de performance (Release sem sanitizers, `MemoryCanvas`, 800×600, 2 aquecimentos + 10 medições, mediana, 100 ms);
  - os manuais remanescentes: janela, `D`, `W`, `1`–`6`, `+`/`-`, `F11`, `Esc`/`Q`, recarga `R` ao vivo, `--test-pattern` e `--fullscreen`.
- Não alterar seções 1–3, 5–7, `README.md` ou ADRs, salvo a seção 4 e a nova tabela de cobertura.

## 4. Seams de teste

| Seam | Teste | O que observa | Justificativa |
|---|---|---|---|
| C++ headless + `MemoryCanvas` | `model_coverage` | Pixels não pretos, cores distintas e variação nos 6 modelos reais | Prova o critério literal sem janela; usa os arquivos reais de câmera/iluminação. |
| Geometria sintética + renderer | `renderer_phong` (função nova) | Face com normal para longe da câmera e luz atrás ainda fica branca | Isola two-sided sem depender de malha real nem de inspeção visual. |
| `MemoryCanvas` + `std::chrono` | `performance` | Mediana de 10 redesenhos por modelo em 800×600 | É o seam exato do RNF06: `renderMesh` sem SDL/janela. |
| Processo/CLI | testes existentes da #26 | Dump iluminado, erros de iluminação e `--smoke` inválido | Já cobrem a issue; não duplicar. |
| CMake/test properties | `performance`, `check` | Otimização do alvo, `RUN_SERIAL`, dependências do `check` | Evita falha local por build não otimizada e flakiness com `ctest -j`. |
| Documento de requisitos | `docs/requisitos-2va.md` | Checkboxes e tabela critério→teste/manual | A issue pede cobertura rastreável dos critérios da 2ª VA. |

## 5. Tarefas

### T1 — Baseline e auditoria (S)

- **Descrição:** reconfirmar a linha de base e registrar os números de auditoria.
- **Critérios de aceitação:** `make test` 30/30; `make smoke` exit 0; `ctest -N` = 30; `git status --short` vazio; métricas de cobertura/performance conferidas com a tabela da seção 3.
- **Verificação:** `make test`; `make smoke`; `ctest --test-dir build -N`; `git status --short`.
- **Dependências:** nenhuma.
- **Arquivos prováveis:** nenhum.
- **Escopo:** S.

### T2 — `tests/test_model_coverage.cpp` + CTest `model_coverage` (M)

- **Descrição:** criar o teste de cobertura dos 6 modelos descrito em 3.1, com câmera local `d = 20`, 800×600, thresholds 500/8/variação e impressão de métricas.
- **Critérios de aceitação:**
  - [ ] Carrega os 6 modelos, `camera/camera.txt` e `lighting/lighting.txt`.
  - [ ] Cada modelo tem `>= 500` pixels não pretos, `>= 8` cores distintas e faixa de canal `>= 2`.
  - [ ] O teste imprime as métricas por modelo para evidência.
  - [ ] Nenhuma chamada SDL; `MemoryCanvas` apenas; a câmera do app não é alterada.
  - [ ] `model_coverage` verde; contagem 31; `WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}` e `TIMEOUT 30`.
- **Verificação:** `cmake --build build --target test_model_coverage`; `ctest --test-dir build -R '^model_coverage$' -V`; comparar a saída com a tabela de 3.1.
- **Dependências:** T1.
- **Arquivos prováveis:** `tests/test_model_coverage.cpp`, `CMakeLists.txt`.
- **Escopo:** M.

### T3 — Two-sided de integração em `renderer_phong` (S)

- **Descrição:** adicionar a função descrita em 3.2 e chamá-la em `main()` do teste; creditar a cobertura two-sided existente e tratar a nova função como regressão focada.
- **Critérios de aceitação:**
  - [ ] A face com normal `+Z` (para longe da câmera) e luz atrás da câmera renderiza branca em um pixel **interior** (ex.: `(500,225)`), não no vértice central.
  - [ ] O controle com winding invertido também renderiza branco.
  - [ ] O pixel interior confere com `phongColor` usando a normal invertida (`{0,0,-1}`), com tolerância ≤ 1.
  - [ ] O teste não usa ambiente nem especular (ambiente 0, `Ks = 0`) para isolar a difusa.
  - [ ] `renderer_phong` continua verde; contagem não muda (31); a cobertura existente é citada na docs.
- **Verificação:** `cmake --build build --target test_renderer_phong`; `ctest --test-dir build -R '^renderer_phong$' -V`.
- **Dependências:** T1.
- **Arquivos prováveis:** `tests/test_renderer_phong.cpp`.
- **Escopo:** S.

### T4 — `tests/test_performance.cpp` + CTest `performance` (M)

- **Descrição:** criar o teste RNF06 descrito em 3.3.
- **Critérios de aceitação:**
  - [ ] Mede os 6 modelos em 800×600, `MemoryCanvas`, sem SDL/janela/dump.
  - [ ] 2 aquecimentos + 10 medições; mediana `<= 100 ms` por modelo; imprime tempos.
  - [ ] Carrega câmera e iluminação padrão; não altera arquivos.
  - [ ] Target compilado com `-O3`/`NDEBUG` quando `CMAKE_BUILD_TYPE` estiver vazio ou for `Debug`; em Release usa os flags globais; `RUN_SERIAL TRUE`; `TIMEOUT 120`.
  - [ ] Relatório registra máquina (CPU, núcleos, RAM), `g++ --version` e flags.
  - [ ] Verde no build local padrão (`make test`) e em build Release.
  - [ ] Contagem 32.
- **Verificação:** `cmake --build build --target test_performance`; `ctest --test-dir build -R '^performance$' -V`; repetir em `build-release` com `-DCMAKE_BUILD_TYPE=Release`.
- **Dependências:** T1.
- **Arquivos prováveis:** `tests/test_performance.cpp`, `CMakeLists.txt`.
- **Escopo:** M.

### T5 — Auditoria de CMake e `check` (S)

- **Descrição:** consolidar os registros CTest, propriedades e dependências do target `check`.
- **Critérios de aceitação:**
  - [ ] `ctest -N` lista exatamente 32 testes, sem duplicatas.
  - [ ] `model_coverage` e `performance` têm `WORKING_DIRECTORY`; `model_coverage` `TIMEOUT 30`; `performance` `TIMEOUT 120` e `RUN_SERIAL`.
  - [ ] `check` depende de `test_model_coverage`, `test_performance` e dos demais alvos.
  - [ ] Build sem warnings com `-Wall -Wextra -Wpedantic`.
- **Verificação:** `ctest --test-dir build -N`; `cmake --build build --target check`; `cmake --build build --clean-first 2>&1 | grep -i warning` (sem saída).
- **Dependências:** T2–T4.
- **Arquivos prováveis:** `CMakeLists.txt`.
- **Escopo:** S.

### T6 — Atualizar `docs/requisitos-2va.md` (M)

- **Descrição:** marcar os 17 critérios e adicionar a tabela de cobertura descrita em 3.5, incluindo #26 e os manuais.
- **Critérios de aceitação:**
  - [ ] Nenhum `[ ]` restante na seção 4.
  - [ ] Cada critério aponta para pelo menos um teste ou verificação manual documentada, distinguindo **automatizado**, **estático** e **manual**.
  - [ ] A tabela cita `model_coverage`, `performance`, `renderer_phong` ampliado, a cobertura two-sided existente (`iluminacao_usa_centro_da_camera_em_mundo`) e os testes de CLI/dump/recarga da #26.
  - [ ] RNF04 cita `sdl_backend_policy` (estático) e a verificação final de AGENTS/#30; R/`--test-pattern`/`--fullscreen` são manuais/estáticos, não superestimados.
  - [ ] Câmera de teste `d = 20` e procedimento de performance estão documentados.
  - [ ] `README.md`, ADRs, `camera/` e `lighting/` intocados.
- **Verificação:** `rg -n '\[ \]' docs/requisitos-2va.md` (sem resultados na seção 4); `git diff -- docs/requisitos-2va.md`; revisão manual da tabela.
- **Dependências:** T2–T5.
- **Arquivos prováveis:** `docs/requisitos-2va.md`.
- **Escopo:** M.

### T7 — Validação final (M)

- **Descrição:** executar a validação completa da issue, incluindo build limpo, suíte, smoke, headless, Release e verificação manual quando houver display.
- **Critérios de aceitação:**
  - [ ] `make clean && make build` sem warnings.
  - [ ] `make test` 32/32; `ctest --test-dir build --output-on-failure` verde.
  - [ ] `make smoke` exit 0; suíte roda com `SDL_VIDEODRIVER=dummy` e sem `DISPLAY`.
  - [ ] Build Release: `performance` verde; tempos dos 6 modelos impressos e ≤ 100 ms.
  - [ ] `git diff -- src` vazio; `git diff --check` limpo.
  - [ ] Verificação manual documentada: `make dev` com os 6 modelos, `D`, `W`, `1`–`6`, `+`/`-`, `F11`, `Esc`/`Q`, `R` ao vivo, `--test-pattern`, `--fullscreen`; se não houver display, registrar a limitação e usar a evidência da #26 + dumps.
- **Verificação:** comandos da seção 8 de validação; `ctest -N` = 32; `git status --short`.
- **Dependências:** T6.
- **Arquivos prováveis:** nenhum.
- **Escopo:** M.

### T8 — Commit único e revisão de escopo (S)

- **Descrição:** criar um único commit em `test/m27` com os arquivos previstos.
- **Critérios de aceitação:**
  - [ ] Commit contém `tasks/m27-plan.md`, `tests/test_model_coverage.cpp`, `tests/test_performance.cpp`, `tests/test_renderer_phong.cpp`, `CMakeLists.txt` e `docs/requisitos-2va.md`.
  - [ ] Fora do commit: `src/`, `README.md`, `docs/adr/`, `camera/`, `lighting/`, `build/`, BMPs.
  - [ ] `git show --stat HEAD` confere; `git diff --check` limpo.
- **Verificação:** `git status --short`; `git add` seletivo; `git show --stat HEAD`; `git diff --check`.
- **Dependências:** T7.
- **Arquivos prováveis:** os listados.
- **Escopo:** S.

### T9 — PR, CI, merge e close da #27 (S)

- **Descrição:** publicar a branch, abrir PR para `dev`, aguardar CI headless, fazer merge e confirmar o fechamento da issue.
- **Critérios de aceitação:**
  - [ ] PR `test/m27` → `dev` aberto com `Closes #27` e resumo (32 testes, métricas, comandos).
  - [ ] CI `build-and-test` verde no PR e, após o merge, em `dev`.
  - [ ] Merge realizado no GitHub (mesmo método da #26); sem merge manual local em `dev`.
  - [ ] Issue #27 fechada; branch remota removida se o padrão do projeto permitir.
- **Verificação:** `gh pr checks --watch`; `gh pr view`; `gh issue view 27`; `git log --oneline --graph` em `dev`.
- **Dependências:** T8.
- **Arquivos prováveis:** nenhum.
- **Escopo:** S.

## 6. Matriz de testes

| CTest | O que prova | Comando | Estado esperado |
|---|---|---|---|
| `model_coverage` (**novo**) | 6 modelos em 800×600 com câmera local `d=20`; ≥500 pixels não pretos, ≥8 cores e variação de cor por modelo | `ctest --test-dir build -R '^model_coverage$' -V` | GREEN após T2; antes inexistente; contagem 31 |
| `renderer_phong` (**ampliado**) | Two-sided de integração: normal para longe da câmera + luz atrás → pixel branco; winding invertido como controle; demais funções preservadas | `ctest --test-dir build -R '^renderer_phong$' -V` | GREEN; não altera a contagem |
| `performance` (**novo**) | RNF06: 6 modelos, 800×600, `MemoryCanvas`, 2 aquecimentos + 10 medições, mediana ≤100 ms, sem SDL | `ctest --test-dir build -R '^performance$' -V` (local e Release) | GREEN após T4; contagem 32 |
| `lighting`, `phong`, `shaded_rasterizer`, `renderer`, `normals` | Parser, equação, interpolação/z-buffer, renderer e normais — cobertura já existente | `ctest --test-dir build -R '^(lighting|phong|shaded_rasterizer|renderer|normals)$'` | GREEN; citar, não duplicar |
| `lighting_padrao`, `lighting_sucesso`, `lighting_inexistente`, `lighting_invalido`, `smoke_iluminacao_invalida`, `lighting_policy`, `camera_padrao`, `dump_sucesso`, `smoke`, `info_*` | Dump iluminado, erros de CLI/iluminação, recarga e caminhos — testes da #26 e da 1ª VA | `ctest --test-dir build --output-on-failure` | GREEN; citar na cobertura |
| `sdl_backend_policy`, `camera_policy`, `default_render_mode` | Política de SDL/pixel, câmera compartilhada e modo inicial | `ctest --test-dir build --output-on-failure` | GREEN; citar na cobertura de RNF04 |

**Contagem final: 30 + `model_coverage` + `performance` = 32 testes.** `renderer_phong` é modificado, não acrescentado. Saída esperada: `100% tests passed, 0 tests failed out of 32`.

## 7. Checkpoints

| Checkpoint | Critérios |
|---|---|
| CP0 — baseline (T1) | `make test` 30/30; `make smoke` exit 0; `ctest -N` = 30; tree limpo; métricas auditadas registradas. |
| CP1 — cobertura (T2–T3) | `model_coverage` verde com as métricas impressas; `renderer_phong` verde com o two-sided; 31 testes; sem SDL. |
| CP2 — performance e CMake (T4–T5) | `performance` verde no build local e Release; 32 testes; `check` verde; sem warnings; `ctest -j` seguro pelo `RUN_SERIAL`. |
| CP3 — documentação (T6) | 17 checkboxes `[x]`; tabela critério→teste/manual completa; README/ADRs intocados. |
| CP4 — validação final (T7) | Build limpo; 32/32; `make smoke`; headless com `SDL_VIDEODRIVER=dummy`; Release com mediana ≤100 ms; `git diff -- src` vazio. |
| CP5 — review/CI/merge (T8–T9) | Commit único com escopo correto; CI verde; PR mergeado; issue #27 fechada. |

## 8. Riscos e mitigações

| Risco | Impacto | Mitigação |
|---|---|---|
| Teste de performance falhar em build não otimizada (`-O0`: `maca` 111,1 ms) | Alto para `make test` local | Compilar `test_performance` com `-O3`/`NDEBUG` quando `CMAKE_BUILD_TYPE` estiver vazio ou for `Debug`; em Release usar os flags globais; medir somente `renderMesh`; documentar que o alvo é otimizado por definição do RNF06. |
| `-O3` gerar warning em `src/` | Médio | Pré-validar o target; se houver warning, abrir issue e não editar/suprimir `src/`; o build normal continua sem warnings. |
| Faixa de canal `< 2` em algum modelo | Baixo | Auditar os 6 com `d=20`: triângulo 5, pirâmide 30, maçã 112, maçã2 100, vaso 215, cálice2 215; o threshold 2 tem margem. |
| Two-sided já coberto e novo teste tautológico | Médio | Creditar `iluminacao_usa_centro_da_camera_em_mundo`; a nova função usa pixel interior e controle de winding; mutação desativa a inversão e deve falhar. |
| Flakiness de tempo no CI | Médio | Mediana de 10 após 2 aquecimentos; margem de ~4× (máx. auditado 25,3 ms); `RUN_SERIAL`; imprimir tempos para diagnóstico. Não relaxar o limite de 100 ms. |
| Câmera de teste `d=20` divergir da câmera do app | Baixo | Carregar `camera/camera.txt` e alterar apenas a cópia local; nunca escrever no arquivo; documentar na tabela de cobertura. |
| Mudança futura de `camera.txt` invalidar os thresholds | Baixo | O teste imprime métricas; recalibrar apenas se a câmera compartilhada mudar por issue própria; os thresholds 500/8 são o requisito, não um baseline de pixel. |
| Duplicação de testes existentes | Médio | Não criar testes de z-buffer, dump, CLI ou parser; citar `shaded_rasterizer`, `renderer_phong`, `lighting_*` e `dump_*` na cobertura. |
| Two-sided passar por acidente (ambiente ou especular) | Médio | Cenário com `Iamb=0`, `Ks=0`, `Kd=1`; normal para longe da câmera e luz atrás; exigir branco e comparar com a normal invertida; controle com winding invertido. |
| Docs #28/#29 | Médio | Não tocar `README.md` nem `docs/adr/`; apenas seção 4 e tabela de cobertura de `docs/requisitos-2va.md`. |
| CI sem display | Baixo | Workflow já define `SDL_VIDEODRIVER=dummy`; novos testes não usam SDL; `smoke`/CLI já usam dummy. Nenhuma mudança no workflow. |
| Contagem de testes divergente | Médio | `ctest -N` = 32 em T5/T7; `check` depende dos novos alvos; `renderer_phong` não é duplicado. |
| `ctest -j` e arquivos temporários | Baixo | `performance` com `RUN_SERIAL`; BMPs têm nomes distintos e ficam em `${CMAKE_BINARY_DIR}` (ignorados por `*.bmp`); novos testes não escrevem arquivos. |
| Commit capturar build/BMP | Baixo | `git status --short` antes do commit; `git add` seletivo; `git diff --check`. |
| Regressão em `src/` | Alto | Proibido alterar `src/`; `git diff -- src` vazio em T7/T8; se um teste expuser bug, reportar e abrir issue separada. |

## 9. Questões abertas e decisões assumidas

Nenhuma questão bloqueante. Decisões tomadas com base nos documentos:

1. **Checkboxes:** todos os 17 critérios da seção 4 serão marcados `[x]` porque cada um tem teste automatizado ou verificação manual documentada; os critérios exclusivos de janela (`--test-pattern`, `--fullscreen`, controles interativos e `R` ao vivo) ficam com procedimento manual documentado e, quando houver display, execução registrada no relatório.
2. **Performance:** o teste é registrado sempre, com alvo otimizado, em vez de existir apenas em Release; assim `make test` local também conta e valida o RNF06 sem esconder o teste.
3. **Two-sided:** permanece dentro de `renderer_phong`, sem novo CTest; a contagem final é 32.
4. **Git/GitHub:** seguir o padrão da #26 — commit único em `test/m27`, PR para `dev`, CI verde, merge no GitHub e fechamento da #27 via `Closes #27`.

## 10. Definition of Done da #27 e escopo do commit/PR

### Definition of Done

- [ ] `model_coverage` e `performance` criados; `renderer_phong` ampliado com two-sided; **32/32 testes verdes**.
- [ ] `cmake --build build` sem warnings (`-Wall -Wextra -Wpedantic`).
- [ ] `make smoke` exit 0; suíte executa headless com `SDL_VIDEODRIVER=dummy`, sem servidor gráfico.
- [ ] `performance` verde no build local e em Release; mediana ≤100 ms para os 6 modelos, com tempos impressos e máquina/compilador/flags registrados.
- [ ] `docs/requisitos-2va.md` com os 17 checkboxes marcados e tabela critério→teste/manual; `README.md` e ADRs intocados.
- [ ] `git diff -- src` vazio; `camera/`, `lighting/`, modelos e build/BMP fora do commit.
- [ ] `ctest -N` = 32; target `check` inclui os novos alvos.
- [ ] Commit único em `test/m27`; PR para `dev`; CI verde; merge realizado; issue #27 fechada.
- [ ] Relatório final no formato do `AGENTS.md`, com arquivos, comandos, resultados, critérios um a um e limitações.

### Escopo do commit e PR

- **Branch:** `test/m27`.
- **Commit único sugerido:** `test: cobre critérios headless da 2ª VA (#27)`.
- **Incluídos:** `tasks/m27-plan.md`, `tests/test_model_coverage.cpp`, `tests/test_performance.cpp`, `tests/test_renderer_phong.cpp`, `CMakeLists.txt`, `docs/requisitos-2va.md`.
- **Fora:** `src/`, `README.md` (#28), `docs/adr/` (#29), `camera/`, `lighting/`, `models/`, `build/`, `*.bmp`.
- **PR:** `test/m27` → `dev`, corpo com resumo, tabela de testes/tempos, comandos de validação e `Closes #27`.
- **Fechamento:** aguardar CI verde, fazer merge pelo GitHub como na #26, confirmar issue fechada e registrar o link do PR no relatório.

## 11. Handoff sugerido

1. **Tester — T2–T5:** implementar `test_model_coverage`, a função two-sided em `test_renderer_phong` e `test_performance`; rodar as métricas e reportar os números impressos. Não alterar `src/`.
2. **Coder — contingência:** só atuar se um teste revelar defeito real ou se o alvo otimizado precisar de ajuste de CMake; nesse caso, limitar-se a `CMakeLists.txt`/testes e reportar qualquer suspeita de bug em `src/` para issue separada.
3. **Reviewer — T6–T9:** revisar a tabela de cobertura, os checkboxes, o escopo do commit, o CI headless, o merge e o fechamento da #27; conferir a contagem 32 e a ausência de mudanças em `src/`, README e ADRs.
