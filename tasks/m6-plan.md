# Plano de implementação — Milestone 6: 2ª VA — CLI, iluminação e recarga com R

Tracker: GitHub issue #26; este documento é o índice ordenado; não há duplicação em `tasks/todo.md`.

## 1. Visão geral e objetivo

A #26 integra a 2ª VA no executável: terceiro posicional opcional `[modelo.byu] [camera.txt] [lighting.txt]` com padrão `lighting/lighting.txt`, carga da iluminação antes do render, `--dump`/`--smoke` no caminho Phong e `R` recarregando câmera **e** iluminação de forma atômica. A mudança fica concentrada em `src/main.cpp` e na suíte de testes; o pipeline existente não é reescrito.

## 2. Não-objetivos

- Não criar ADR-0010 (escopo de #29), não alterar ADR-0006 e não reescrever o README da 2ª VA (#28).
- Não marcar checkboxes de `docs/requisitos-2va.md` (#27) nem medir desempenho/pixels dos 6 modelos (#27).
- Não alterar `Renderer`, `Lighting`, `Camera`, `Mesh`, `Normals`, `Canvas`, `Rasterizer`, `ShadedRasterizer` ou `Phong`; não duplicar pipeline no `main`; não mudar `Points`/`Wireframe`; sem dependências, primitivas SDL ou flags novas.

## 3. Decisões de arquitetura

### O que não pode mudar

| Módulo/contrato | Restrição |
|---|---|
| `Renderer` | Intocado. A sobrecarga iluminada é o único ponto de integração; `Points`/`Wireframe` continuam delegando ao legado (`Renderer.cpp`, 131–134). |
| `Lighting` | Intocado. Parser estrito e atômico (`lighting = loaded` só no fim). |
| `Camera` | Intocado. `loadCameraFromFile` já é atômico. |
| `Mesh`/`Normals` | Intocados. `1`–`6` troca só a malha; câmera e iluminação são compartilhadas. |
| `Canvas`/`MemoryCanvas`/`WindowSDL` | Intocados. Contrato `clear` + `drawPixel`. |
| `Points`/`Wireframe` | Sem iluminação e sem profundidade. |
| Caminho legado | `renderMesh` sem `Lighting` permanece para testes da 1ª VA e depuração. |

### Integração no `main`

Sempre chamar a sobrecarga iluminada; o `Renderer` decide entre Phong e legado pelo modo:

```cpp
cg::renderMesh(mesh, camera, lighting, canvas);        // --dump (Filled)
cg::renderMesh(mesh, camera, lighting, window, mode);  // janela e --smoke
```

Sem `if (mode == Filled)` no `main`, sem chamar `fillTriangleShaded`/`DepthBuffer`/`phongColor` diretamente. `--test-pattern` continua sendo o único caminho que não renderiza a malha.

### Decisões registradas (sem questões abertas bloqueantes)

| # | Decisão |
|---|---|
| D1 | Iluminação ausente/inválida no startup → `EXIT_FAILURE` + `stderr` (consistente com modelo/câmera). |
| D2 | `--test-pattern` continua carregando câmera e iluminação no startup; documentar como esperado. |
| D3 | `--info` continua retornando antes das cargas; não depende de `lighting/` nem `camera/`. |
| D4 | `R` tenta câmera e iluminação independentemente; redesenha se `cameraReloaded \|\| lightingReloaded`; ambas falhando, não redesenha (RF15). |
| D5 | `--dump` sempre `Filled` iluminado; `--test-pattern` só afeta a janela. |
| D6 | `CONTEXT.md`: `Recarga da câmera` → `Recarga da configuração` (texto exato em T8). |
| D7 | Sem mudanças em README, ADRs e `docs/requisitos-2va.md` (#28/#29/#27). |
| D8 | `tasks/m6-plan.md` entra no commit: planos anteriores estão versionados e este é o índice da issue. |
| D9 | `lighting_padrao`: dump A **sem posicionais** (trio padrão completo), dump B explícito com os três arquivos; A == B cobre diretamente o primeiro critério de aceitação. Dump C usa `lighting-pdf.txt` e deve diferir de B. |
| D10 | Testes de CLI usam `tests/check_lighting_error.cmake`/`check_lighting_success.cmake` com `execute_process`, **não** `WILL_FAIL TRUE` + `PASS_REGULAR_EXPRESSION`: essas propriedades não se compõem no CTest (caso correto com exit 1 falha; caso errado com exit 1 passa). O script de erro exige exit != 0, mensagem literal no `stderr` e ausência de BMP; o de sucesso exige exit 0, `BMP salvo` e BMP existente. |
| D11 | A comparação B vs C é válida sem pixel hardcoded: `piramide` com `lighting.txt` difere de `lighting-pdf.txt` em 463 pixels, verificado com harness headless no mesmo modelo e câmera. |
| D12 | Cobertura extra do architect: `info_fora_do_diretorio` (regressão verde; prova `--info` antes das cargas e independente do CWD) e `smoke_iluminacao_invalida` (prova o startup iluminado no caminho `--smoke`). Total: 23 + 7 = 30 testes. |

## 4. Seams de teste

| Seam | Teste | O que observa | Justificativa |
|---|---|---|---|
| Processo/CLI | `lighting_inexistente`, `lighting_invalido` | exit != 0, `stderr` com caminho e mensagem, sem BMP | Startup do `main` só é observável no processo. |
| Processo/CLI (smoke inválido) | `smoke_iluminacao_invalida` | exit != 0, `stderr` com a mensagem do parser, sem `BMP salvo` | Prova que o startup iluminado vale também para `--smoke`, não só para `--dump`. |
| Processo/CLI (sucesso) | `lighting_sucesso` | exit 0, `BMP salvo` no stdout e BMP existente/não vazio com `models/maca.byu` + `lighting-pdf.txt` | Prova o critério literal da issue: o terceiro arquivo é aceito, o dump é gravado e o processo retorna 0. |
| Processo/CLI (`--info`) | `info_fora_do_diretorio` | contagens impressas a partir de `${CMAKE_BINARY_DIR}` com caminho absoluto | Prova que `--info` continua antes das cargas e não depende de `lighting/` nem `camera/`. |
| Dump BMP | `lighting_padrao` | A (sem posicionais) == B (três explícitos); B != C (`lighting-pdf.txt`, 463 pixels); `EXISTS` de A/B/C | Determinístico, headless, sem pixel hardcoded; cobre o trio padrão completo e prova o caminho Phong. |
| Estática do `main` | `lighting_policy` | default `lighting/lighting.txt`, `loadLightingFromFile`, bloco `SDLK_r` isolado até o próximo `} else if` com `loadCamera(cameraPath, camera)`, `loadLighting(lightingPath, lighting)`, `cameraReloaded \|\| lightingReloaded` e `redraw()`, sem `&&`/`return`/`exit`; e as duas chamadas iluminadas (`canvas` no dump, `window, mode` na janela/smoke) | `R` depende de evento SDL; sem isolar o bloco e checar as chamadas de render, `--smoke` poderia continuar legado ou o `R` poderia carregar sem redesenhar. |
| C++ existentes | `renderer_phong`, `lighting`, etc. | pipeline iluminado e parser | Já cobrem o interno; não duplicar. |

Não testar nomes de variáveis/helpers além de símbolos estáveis (`SDLK_r`, prefixos `loadCamera`/`loadLighting`) e das chamadas de render exigidas pela issue.

## 5. Tarefas

### T1 — Baseline (S)

- **Descrição:** reconfirmar a linha de base.
- **Critérios:** `make test` 23/23; `make smoke` exit 0; `ctest -N` = 23; `git status --short` vazio.
- **Verificação:** `make test`, `make smoke`, `ctest --test-dir build -N`.
- **Dependências:** nenhuma.

### T2 — `tests/check_lighting_policy.cmake` + `lighting_policy` (S)

- **Mudança:** exigir `PROJECT_SOURCE_DIR`; ler `src/main.cpp`; falhar sem `lighting/lighting.txt` e sem `loadLightingFromFile`; localizar `SDLK_r`, extrair o bloco até o próximo `} else if` (via `string(SUBSTRING ...)` + `string(FIND)` no restante) e exigir dentro dele:
  - `loadCamera(cameraPath, camera)` e `loadLighting(lightingPath, lighting)` em chamadas separadas;
  - a condição `cameraReloaded || lightingReloaded`;
  - `redraw()`;
  - ausência de `&&`, `return` e `exit` (erro de recarga não pode encerrar o app nem curto-circuitar as cargas).
  Exigir também, no arquivo inteiro:
  - `renderMesh(mesh, camera, lighting, canvas)` (dump iluminado);
  - `renderMesh(mesh, camera, lighting, window, mode)` (janela/`--smoke` iluminados).
  Usar regex tolerante a espaços/quebras de linha nas chamadas de render.
- **CMake:** `add_test(NAME lighting_policy COMMAND ${CMAKE_COMMAND} -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR} -P .../check_lighting_policy.cmake)` com `WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}` e `TIMEOUT 10`.
- **Critérios:** teste existe e falha apontando default/recarga/render ausente (red).
- **Verificação:** `ctest --test-dir build -R '^lighting_policy$' --output-on-failure`.
- **Dependências:** T1.
- **Escopo:** S.

### T3 — `tests/check_default_lighting.cmake` + `lighting_padrao` (S)

- **Mudança:** três dumps:
  - A (padrão completo, **sem posicionais**): `cpu-renderer --dump dump-lighting-padrao.bmp`;
  - B (explícito): `cpu-renderer --dump dump-lighting-explicito.bmp models/piramide.byu camera/camera.txt lighting/lighting.txt`;
  - C (PDF): `cpu-renderer --dump dump-lighting-pdf.bmp models/piramide.byu camera/camera.txt lighting/lighting-pdf.txt`.
  Exigir A, B e C com exit 0; exigir `EXISTS` dos três BMPs antes de comparar; exigir `cmake -E compare_files` **igual** entre A e B (cobre piramide + `camera/camera.txt` + `lighting/lighting.txt`) e **diferente** entre B e C. A diferença é de 463 pixels (D11), então a comparação byte a byte é robusta e não depende de pixel hardcoded; o `EXISTS` impede que um C ausente passe como “diferente”.
- **CMake:** `lighting_padrao` com `-DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>`, `-DPROJECT_SOURCE_DIR`, `-DOUTPUT_DIR=${CMAKE_BINARY_DIR}`.
- **Critérios:** falha hoje porque B é rejeitado (terceiro posicional) (red); após T6, A == B e B != C.
- **Verificação:** `ctest --test-dir build -R '^lighting_padrao$' --output-on-failure`.
- **Dependências:** T1.
- **Escopo:** S.

### T4 — `tests/check_lighting_error.cmake` + `lighting_inexistente`/`lighting_invalido`/`smoke_iluminacao_invalida` (S)

- **Mudança:** script com `-DCASO=ausente|invalido|smoke_invalido`:
  - `ausente` → `tests/fixtures/lighting-inexistente.txt`, mensagem esperada `não foi possível abrir`, modo `--dump`;
  - `invalido` → `tests/fixtures/lighting-range-invalido.txt`, mensagem esperada `Ka deve estar em [0,1]`, modo `--dump`;
  - `smoke_invalido` → `tests/fixtures/lighting-range-invalido.txt`, mesma mensagem, modo `--smoke` com `SDL_VIDEODRIVER=dummy`;
  - remover o BMP do caso (`dump-lighting-erro-ausente.bmp` / `dump-lighting-erro-invalido.bmp`; `smoke_invalido` não gera BMP);
  - `execute_process` com `--dump <bmp> models/triangulo.byu camera/camera.txt <caminho>` nos dois primeiros e `cmake -E env SDL_VIDEODRIVER=dummy cpu-renderer --smoke models/piramide.byu camera/camera.txt <caminho>` no terceiro, capturando `RESULT_VARIABLE`, `OUTPUT_VARIABLE`, `ERROR_VARIABLE`;
  - exigir exit != 0; usar `string(FIND)` (literal, não regex) para caminho e mensagem no `stderr`; exigir `stdout` sem `BMP salvo`; nos casos `--dump`, exigir BMP ausente.
- **CMake:** três `add_test` com `-DCASO=ausente`/`-DCASO=invalido`/`-DCASO=smoke_invalido`, `-DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>`, `-DOUTPUT_DIR=${CMAKE_BINARY_DIR}`, `WORKING_DIRECTORY` e `TIMEOUT 10`.
- **Critérios:** ambos falham hoje pelo motivo certo — o `main` atual diz `argumento desconhecido`, sem `não foi possível abrir` nem `Ka deve estar em [0,1]` (red).
- **Verificação:** `ctest --test-dir build -R '^(lighting_(inexistente|invalido)|smoke_iluminacao_invalida)$' --output-on-failure`.
- **Dependências:** T1.
- **Escopo:** S.

### T5 — `tests/check_lighting_success.cmake` + `lighting_sucesso` (S)

- **Descrição:** registrar o teste de aceitação exato da issue para o terceiro arquivo, verificando exit code, mensagem e BMP.
- **Mudança:** criar `tests/check_lighting_success.cmake` que:
  - exige `CPU_RENDERER`, `PROJECT_SOURCE_DIR` e `OUTPUT_DIR`;
  - remove `dump-lighting-sucesso.bmp`;
  - roda `--dump <bmp> models/maca.byu camera/camera.txt lighting/lighting-pdf.txt` com `execute_process`;
  - exige `RESULT_VARIABLE == 0`, `BMP salvo` no stdout, `EXISTS` do BMP e tamanho maior que o cabeçalho.
- **CMake:**
  ```cmake
  add_test(NAME lighting_sucesso
           COMMAND ${CMAKE_COMMAND}
                   -DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>
                   -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                   -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
                   -P ${CMAKE_SOURCE_DIR}/tests/check_lighting_success.cmake)
  set_tests_properties(lighting_sucesso PROPERTIES
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      TIMEOUT 10
  )
  ```
  `PASS_REGULAR_EXPRESSION` sozinho não serve: o CTest ignora o exit code nesse modo.
- **Critérios:** falha hoje (terceiro posicional rejeitado, sem `BMP salvo`) e passa após T6; cobre exatamente `./build/cpu-renderer models/maca.byu camera/camera.txt lighting/lighting-pdf.txt`.
- **Verificação:** `ctest --test-dir build -R '^lighting_sucesso$' --output-on-failure`.
- **Dependências:** T1.
- **Escopo:** S.

### T5b — `info_fora_do_diretorio` (S)

- **Descrição:** garantir que `--info` continua antes das cargas e não depende de câmera/iluminação.
- **CMake:**
  ```cmake
  add_test(NAME info_fora_do_diretorio
           COMMAND cpu-renderer --info ${CMAKE_SOURCE_DIR}/models/triangulo.byu)
  set_tests_properties(info_fora_do_diretorio PROPERTIES
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      PASS_REGULAR_EXPRESSION "3 vértices, 1 triângulo"
      TIMEOUT 10
  )
  ```
- **Critérios:** verde já antes da implementação (regressão); se `--info` passar a carregar iluminação, o caminho padrão relativo não existe em `${CMAKE_BINARY_DIR}` e o teste falha.
- **Verificação:** `ctest --test-dir build -R '^info_fora_do_diretorio$' --output-on-failure`.
- **Dependências:** T1.
- **Escopo:** S.

### T6 — `main.cpp`: CLI, default, carga, dump/smoke Phong (M)

- **Mudança:** incluir `shading/Lighting.h`; `kDefaultLightingPath = "lighting/lighting.txt"`; `lightingPath`; terceiro posicional (`positionalCount == 2`, quarto continua erro); helper `loadLighting` com `erro: ...` no `stderr` e atomicidade do loader; carregar modelo → câmera → iluminação com `EXIT_FAILURE` em falha; manter `--info` antes das cargas; dump com `renderMesh(mesh, camera, lighting, canvas)` e `redraw` com `renderMesh(mesh, camera, lighting, window, mode)`; controles intactos.
- **Critérios:** `lighting_padrao`, `lighting_sucesso`, `lighting_inexistente`, `lighting_invalido` e `smoke_iluminacao_invalida` verdes; `info_fora_do_diretorio` continua verde; 23 antigos verdes; `--dump` iluminado; `--smoke` exit 0.
- **Verificação:** `cmake --build build`; `ctest --test-dir build -R 'lighting_|smoke_iluminacao_invalida|info_fora_do_diretorio'`; `ctest` completo (esperado 29/30, só `lighting_policy` vermelho); `make smoke`.
- **Dependências:** T2–T5.
- **Escopo:** M.

### T7 — `main.cpp`: `R` atômico (S)

- **Mudança:**
  ```cpp
  } else if (key == SDLK_r) {
      const bool cameraReloaded = loadCamera(cameraPath, camera);
      const bool lightingReloaded = loadLighting(lightingPath, lighting);
      if (cameraReloaded || lightingReloaded) {
          redraw();
      }
  }
  ```
  As duas cargas são tentadas independentemente; cada helper reporta erro no `stderr`; não há `return`/`exit` no bloco; trocar de malha não recarrega configuração.
- **Critérios:** `lighting_policy` verde; 30/30; manual com uma fonte válida e outra inválida preserva a última válida de cada uma e não derruba o app.
- **Verificação:** `ctest --test-dir build -R '^lighting_policy$'`; `ctest` completo.
- **Dependências:** T6.
- **Escopo:** S.

### T8 — `CONTEXT.md`: termo de recarga (S)

- **Mudança:** substituir `Recarga da câmera` por:
  ```markdown
  **Recarga da configuração**:
  Releitura dos arquivos correntes de câmera e iluminação em tempo de execução, sem reiniciar a aplicação; cada arquivo é carregado atomicamente e, em caso de erro, o último valor válido daquela fonte é mantido.
  _Avoid_: recarga da câmera
  ```
- **Critérios:** nenhuma outra alteração no glossário; `Câmera compartilhada` permanece.
- **Verificação:** `rg "Recarga da" CONTEXT.md`; `git diff CONTEXT.md`.
- **Dependências:** T7.
- **Escopo:** S.

### T9 — Auditoria do CMake (S)

- **Critérios:** 7 testes novos com nomes exatos (`lighting_policy`, `lighting_padrao`, `lighting_sucesso`, `lighting_inexistente`, `lighting_invalido`, `smoke_iluminacao_invalida`, `info_fora_do_diretorio`), `WORKING_DIRECTORY`/`TIMEOUT`, `ctest -N` = 30 sem duplicatas; `check` continua dependendo de `cpu-renderer` (sem novo executável C++).
- **Verificação:** `ctest --test-dir build -N`; `cmake --build build --target check`.
- **Dependências:** T2–T8.
- **Escopo:** S.

### T10 — Validação final, relatório e commit (M)

- **Critérios:** seção 8 + DoD; relatório no formato do AGENTS; commit local na `feat/m6`; sem push/PR.
- **Verificação:** comandos da seção 8; `git diff --check`; `git status --short`; `git show --stat HEAD`.
- **Dependências:** T1–T9.
- **Escopo:** M.

## 6. Matriz de testes

| CTest | O que prova | Antes | Depois |
|---|---|---|---|
| `lighting_policy` | default `lighting/lighting.txt`, loader, bloco `SDLK_r` isolado com `loadCamera(cameraPath, camera)` + `loadLighting(lightingPath, lighting)` + `cameraReloaded \|\| lightingReloaded` + `redraw()`, sem `&&`/`return`/`exit`, e as duas chamadas iluminadas (dump `canvas`; janela/`--smoke` `window, mode`) | RED (sem default/loader/render; após T6, RED só pelo bloco `R`) | GREEN |
| `lighting_padrao` | A (sem posicionais) == B (três explícitos), cobrindo o trio padrão completo; B != C (`lighting-pdf.txt`, 463 pixels); `EXISTS` de A/B/C | RED (B rejeitado) | GREEN |
| `lighting_sucesso` | `--dump ... models/maca.byu camera/camera.txt lighting/lighting-pdf.txt` sai 0 e imprime `BMP salvo` | RED (terceiro posicional rejeitado) | GREEN |
| `lighting_inexistente` | exit != 0, `stderr` com caminho + `não foi possível abrir`, sem BMP | RED (`argumento desconhecido`) | GREEN |
| `lighting_invalido` | exit != 0, `stderr` com caminho + `Ka deve estar em [0,1]`, sem BMP | RED (`argumento desconhecido`) | GREEN |
| `smoke_iluminacao_invalida` | exit != 0, `stderr` com `Ka deve estar em [0,1]`, sem `BMP salvo` no `--smoke` | RED (`argumento desconhecido`) | GREEN |
| `info_fora_do_diretorio` | `--info` de `${CMAKE_BINARY_DIR}` com caminho absoluto imprime as contagens | GREEN (regressão) | GREEN |

Os 23 testes existentes permanecem verdes; `dump_sucesso`, `smoke` e `camera_padrao` passam a exercitar o caminho iluminado. **Contagem final: 23 + 7 = 30 testes** (`100% tests passed, 0 tests failed out of 30`). BMPs em `${CMAKE_BINARY_DIR}`, ignorados por `*.bmp`.

## 7. Checkpoints

| Checkpoint | Critérios |
|---|---|
| CP0 — baseline (T1) | `make test` 23/23; `make smoke` exit 0; `ctest -N` = 23; tree limpo. |
| CP1 — red (T2–T5b) | Build OK; 6 testes novos vermelhos pelos motivos esperados (`lighting_policy`, `lighting_padrao`, `lighting_sucesso`, `lighting_inexistente`, `lighting_invalido`, `smoke_iluminacao_invalida`); `info_fora_do_diretorio` verde desde já; 23 antigos verdes. |
| CP2 — green parcial (T6) | `lighting_padrao`, `lighting_sucesso`, `lighting_inexistente`, `lighting_invalido` e `smoke_iluminacao_invalida` verdes; 29/30; `lighting_policy` vermelho só pelo bloco `R`; `make smoke` exit 0. |
| CP3 — green completo (T7–T9) | 30/30; build sem warnings; `lighting_policy` verde; `CONTEXT.md` atualizado; `check` verde. |
| CP4 — review/visual/commit (T10) | `make dev`; `R` com fontes válidas/inválidas; dumps visuais; `git diff --check`; commit local criado. |

## 8. Validação final

```sh
make clean && make build
make test
make smoke
ctest --test-dir build -R "lighting_|smoke_iluminacao_invalida|info_fora_do_diretorio" --output-on-failure
cmake --build build --target check
```

Manual:

```sh
./build/cpu-renderer
./build/cpu-renderer models/maca.byu camera/camera.txt lighting/lighting-pdf.txt
./build/cpu-renderer --dump build/m6-piramide.bmp
./build/cpu-renderer --dump build/m6-maca-pdf.bmp models/maca.byu camera/camera.txt lighting/lighting-pdf.txt
./build/cpu-renderer --info models/vaso.byu
(cd /tmp && /home/emanueljtrodrigues/Documents/bcc/comp_grafica/cpu-renderer-cpp/build/cpu-renderer \
    --info /home/emanueljtrodrigues/Documents/bcc/comp_grafica/cpu-renderer-cpp/models/vaso.byu)
SDL_VIDEODRIVER=dummy ./build/cpu-renderer --smoke
SDL_VIDEODRIVER=dummy ./build/cpu-renderer --smoke models/piramide.byu camera/camera.txt \
    tests/fixtures/lighting-range-invalido.txt   # deve falhar com mensagem, sem abrir janela
```

Recarga com cópias temporárias:

```sh
cp camera/camera.txt /tmp/m6-camera.txt
cp lighting/lighting.txt /tmp/m6-lighting.txt
make dev ARGS="models/maca.byu /tmp/m6-camera.txt /tmp/m6-lighting.txt"
```

Roteiro: alterar `Il` e `R` → iluminação muda; `Ka = 2` e `R` → erro no `stderr`, iluminação anterior mantida, app vivo; alterar `d` e `R` → câmera muda; invalidar câmera com iluminação válida → câmera mantida, iluminação atualiza e redesenha. Conferir `D`/`W` brancos, `1`–`6` só malha, `+`/`-`, `F11`, `--test-pattern` e `--fullscreen`. A recarga pode ser validada com injeção X11 local (`SDL_VIDEO_X11_XINPUT2=0` + helper fora do repo) e capturas de tela antes/depois; as capturas e o `stderr` são a evidência de CP4.

## 9. Riscos e mitigações

| Risco | Mitigação |
|---|---|
| Teste de pixel frágil | Comparar BMPs da mesma execução; pipeline determinístico/single-thread; B vs C difere em 463 pixels. |
| Policy acoplada a nomes de variáveis | Observar só `SDLK_r`, prefixos `loadCamera`/`loadLighting`, `redraw` e as chamadas de render exigidas pela issue. |
| Janela de 800 caracteres não cobrir `R` | Bloco extraído até o próximo `} else if`; manter câmera→iluminação compacto e sem helper externo. |
| `--smoke` continuar legado | `lighting_policy` exige `renderMesh(mesh, camera, lighting, window, mode)`. |
| Dump continuar legado | `lighting_policy` exige `renderMesh(mesh, camera, lighting, canvas)`; `lighting_padrao` B vs C. |
| Dump A falhar por default de modelo/câmera, não de iluminação | `camera_padrao` isola a câmera e `dump_sucesso` cobre modelo/câmera padrão; A == B continua sendo o critério do trio. |
| `--info` passar a exigir iluminação | Manter retorno antecipado; testar de `/tmp` com caminho absoluto da malha. |
| `--smoke`/`--dump` regredirem | `smoke`, `dump_sucesso`, `dump_modelo_inexistente`, `camera_padrao` + dumps manuais. |
| Atomicidade parcial no `R` | Tentativas independentes; loaders atribuem só no fim; policy exige `\|\|` e ausência de `&&`; validação com injeção X11 local e capturas antes/depois. |
| `WILL_FAIL` + regex no CTest | Proibido por D10; `check_lighting_error.cmake`/`check_lighting_success.cmake` com `execute_process` e `string(FIND)`. |
| README desatualizado | Linha 111 fica falsa após a #26; deixar para #28 e registrar no relatório/commit. |
| ADR-0006 incompleta / sem ADR-0010 | #29; registrar limitação. |
| `--test-pattern` exige `lighting/` | D2; arquivo padrão entregue; documentar. |
| `R` descarta zoom de `+`/`-` | Comportamento existente; registrar limitação. |
| Teste duplicado/sem `WORKING_DIRECTORY` | T9 audita `ctest -N` e `check`. |

## 10. Definition of Done da #26

- [ ] 7 testes novos verdes; `ctest` 30/30; nenhum teste antigo alterado.
- [ ] `cmake --build build` sem warnings (`-Wall -Wextra -Wpedantic`).
- [ ] `make smoke` exit 0; `--smoke` com iluminação inválida falha com mensagem e não abre janela.
- [ ] CLI padrão (sem posicionais) e com três posicionais funcionam; `--dump` iluminado; `--info` não depende de iluminação nem do diretório corrente.
- [ ] `R` recarrega câmera e iluminação independentemente; erro preserva o último valor válido, vai para `stderr` e não derruba o app.
- [ ] `D`, `W`, `+`/`-`, `1`–`6`, `F11`, `Esc`/`Q` preservados; `Points`/`Wireframe` sem iluminação/profundidade.
- [ ] `Renderer`, `Lighting`, `Camera`, `Mesh`, `Normals`, `Canvas`, `Rasterizer`, `ShadedRasterizer` e `Phong` não alterados.
- [ ] `CONTEXT.md` atualizado; README, ADRs e `docs/requisitos-2va.md` intocados.
- [ ] Nenhuma ADR-0010, dependência ou primitiva SDL nova; nenhum BMP versionado.
- [ ] Relatório final no formato do AGENTS, com confirmação de que não houve push/PR.

## 11. Escopo do commit

Um único commit local em `feat/m6`, sem PR/push:

```text
feat(app): carrega iluminação, aceita terceiro arquivo e recarrega com R (#26)
```

Incluídos: `src/main.cpp`, `CMakeLists.txt`, `tests/check_lighting_policy.cmake`, `tests/check_default_lighting.cmake`, `tests/check_lighting_error.cmake`, `tests/check_lighting_success.cmake`, `CONTEXT.md`, `tasks/m6-plan.md`. Fora: `README.md` (#28), `docs/adr/*` (#29), `docs/requisitos-2va.md` (#27), BMPs/build e `/tmp`. `tasks/m6-plan.md` entra porque os planos anteriores estão versionados e ele é o índice ordenado da issue.

## 12. Handoff sugerido

1. **Tester:** T2–T5b (scripts + registros CTest), reportando as seis falhas red pelos motivos esperados (`info_fora_do_diretorio` é regressão verde desde já).
2. **Coder:** T6–T9 com o pseudocódigo e as restrições da seção 3; validar CP2 (29/30) e CP3 (30/30).
3. **Reviewer:** T10, conferindo matriz, roteiro de `R`, BMPs, DoD e commit único; sem push/PR sem autorização.
