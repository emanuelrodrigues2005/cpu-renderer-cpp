Tracker: GitHub issue #28; este documento é o índice ordenado; não há duplicação em `tasks/todo.md`.

# Plano de implementação — Issue #28: README e CONTEXT da 2ª VA

## 1. Visão geral e objetivo

Atualizar a documentação de entrada do projeto para a 2ª VA: o README deixa de descrever a 1ª VA/preenchimento branco e passa a documentar o pipeline completo (normais, profundidade, z-buffer, interpolação perspectiva-correta e Phong por pixel), a CLI de três arquivos, a recarga `R` de câmera + iluminação, os formatos de configuração, as equações/casos especiais, os 32 testes e os links válidos. O CONTEXT ganha/completa os termos canônicos da 2ª VA, sem detalhes de implementação. É uma issue **docs-only**: nenhum código, teste, CMake, câmera, iluminação ou ADR é alterado.

A branch é a atual, **`dev`** (a pedido do responsável); não haverá PR para `dev`. Ao final, um único PR `dev` → `main` leva todo o trabalho da 2ª VA (#26–#28).

## 2. Não-objetivos

- Não alterar `src/`, `tests/`, `CMakeLists.txt`, `camera/`, `lighting/`, `models/`.
- Não criar/alterar ADRs (#29) nem `AGENTS.md` (#30); o README pode linkar o AGENTS existente e registrar a verificação final como #30.
- Não criar um PR para `dev`; o PR final é `dev` → `main`.
- Não incluir a alteração local não relacionada de `camera/camera.txt`; o padrão versionado é `d = 5`, `hx = 2`, `hy = 2`, `C = 0 -500 500`.
- Não prometer comportamento inexistente: sem clipping de plano próximo, sem backface culling, sem múltiplas luzes, sombras, texturas, transparência, flat/Gouraud ou presets de câmera por malha.

## 3. Decisões de documentação

### 3.1 README

Estrutura proposta (substituindo a atual):

1. **Título/introdução da 2ª VA** — pipeline manual em CPU; BYU → câmera → projeção → normais → scan line com `invZ`/z-buffer → Phong por pixel → Canvas; SDL2 apenas janela/pixel; 1ª VA como base, 2ª VA como entrega atual.
2. **Requisitos** — CMake ≥3.16, C++17, SDL2.
3. **Build e execução** — `make build/dev/run/smoke/test/clean`, CMake puro, `ctest`, `--dump`, `--smoke`, `--info`, `--test-pattern`, `--fullscreen`.
4. **CLI e controles** — `[modelo.byu] [camera.txt] [lighting.txt]`; defaults; `R` recarrega câmera **e** iluminação; `1–6` só malha; `D/W/+/-/F11/Esc/Q`; `Points`/`Wireframe` são depuração sem iluminação/profundidade.
5. **Formato da câmera** — seis campos, base `(U,V,N)`, `C`, `d/hx/hy`, validação.
6. **Formato da iluminação** — oito campos `Iamb`, `Ka`, `Il`, `Pl`, `Kd`, `Od`, `Ks`, `eta`; faixas; finitos; parser estrito; `lighting/lighting.txt` padrão; `lighting/lighting-pdf.txt` exemplo do PDF (não padrão).
7. **Pipeline** — etapas, `invZ`, z-buffer (maior `invZ` vence; empate mantém o primeiro), interpolação perspectiva-correta, normal renormalizada/fallback, sem clipping/backface culling.
8. **Iluminação de Phong** — `L = normalize(Pl−P)`, `V = normalize(C−P)`, `R = 2(N·L)N−L`; `Ia`, `Id`, `Is`; especular sem `Od`; two-sided (a)(b)(c); clamp `[0,255]` com arredondamento, sem gamma.
9. **Testes e verificação** — 32 testes, agrupados; `make smoke`; `ctest`; `model_coverage`/`performance`; contagem 32.
10. **Critérios de aceitação da 2ª VA** — resumo + link para `docs/requisitos-2va.md`.
11. **Estrutura do repositório** — `math`, `mesh`, `pipeline`, `shading`, `canvas`, `models`, `camera`, `lighting`, `tests`, `docs`.
12. **Documentação** — links válidos para `CONTEXT.md`, especificações/requisitos 1ª/2ª VA, ADRs 0001–0007/0009 e `AGENTS.md`; registrar que 0008/0010 são #29 e a verificação final de AGENTS é #30.

### 3.2 CONTEXT

- Intro menciona a 2ª VA (normais, z-buffer, Phong).
- Manter os termos já corretos: `Z-buffer`, `invZ`, `Interpolação perspectiva-correta`, `Normal de face`, `Normal de vértice`, `Iluminação de Phong`, componentes, `Fonte de luz`, `Material`, `Vetor L/V/R`, `Modelo de tonalização`, `Tonalização de Phong`, `Câmera compartilhada`.
- Adicionar `Arquivo de iluminação`: arquivo texto com os oito parâmetros; padrão `lighting/lighting.txt`; `lighting/lighting-pdf.txt` preserva o exemplo do PDF.
- `Recarga da configuração`: manter o termo (alinhado a `docs/requisitos-2va.md`), mas explicitar “também chamada de recarga da cena”; câmera + iluminação, atomicidade por fonte, `R`.
- `Câmera compartilhada`: ajustar “`R` a relê” para “`R` recarrega câmera e iluminação”.
- `Canvas`: incluir `clear`, `drawPixel`, `width()` e `height()`.
- `Scan line`: garantir que **não** haja `_Avoid_: z-buffer` (o termo atual tem `_Avoid_: flood fill`; manter flood fill).
- `Renderização iluminada`/`Caminho legado`: remover “sobrecarga de `renderMesh`” (detalhe de implementação) e descrever no nível de domínio.
- Sem detalhes de parser/implementação; sem contradizer o código.

## 4. Matriz requisito → documento → verificação

| Requisito da #28 | README | CONTEXT | Verificação |
|---|---|---|---|
| Título/introdução 2ª VA | §1 | intro | `grep -n "1ª VA" README` não deve tratar como comportamento atual |
| Build/run com os dois arquivos de configuração + modelo (três posicionais) | §3–4 | — | comandos executáveis; `--dump`/`--smoke` com 3 posicionais |
| Controles (`R` câmera+iluminação) | §4 | `Recarga da configuração`, `Câmera compartilhada` | texto + testes `lighting_policy`/`lighting_*` |
| Formato câmera | §5 | `Câmera`/`Base`/`C` | `camera/camera.txt` versionado |
| Formato iluminação | §6 | `Arquivo de iluminação` | `lighting/lighting.txt`, parser/testes `lighting` |
| Pipeline/Phong | §7–8 | termos | `docs/requisitos-2va.md` §3.1–3.5 |
| Testes 32 | §9 | — | `ctest -N` = 32 |
| Critérios 2ª VA | §10 | — | link `docs/requisitos-2va.md` |
| Estrutura/links | §11–12 | — | script de links + `ls` |
| Sem presets por malha | §4/§12 | `Câmera compartilhada` | `grep -n "preset" README.md` |
| CONTEXT sem contradição | — | todos | inspeção contra `src/`/testes |

## 5. Tarefas

### T1 — Auditoria docs (S)

- **Descrição:** tester audita README/CONTEXT contra a #28 e o código; lista gaps, links, contagem, afirmações obsoletas.
- **Critérios:** tabela item→estado→gap; 32 testes; links quebrados/ausentes; `camera` local ignorada.
- **Verificação:** `ctest -N`; `grep`/`ls`; `git show HEAD:camera/camera.txt`.
- **Dependências:** nenhuma.
- **Arquivos:** nenhum.

### T2 — README (M)

- **Descrição:** reescrever as seções 1–12 de 3.1.
- **Critérios:** não trata 1ª VA/branco como atual; CLI/controles corretos; formatos câmera/iluminação; pipeline/equações/two-sided/clamp; 32 testes; critérios; links válidos; sem presets.
- **Verificação:** leitura + `grep` de termos obsoletos; comandos dos exemplos; links.
- **Dependências:** T1.
- **Arquivos:** `README.md`.

### T3 — CONTEXT (M)

- **Descrição:** aplicar 3.2, adicionar `Arquivo de iluminação`, ajustar `Canvas`, `Câmera compartilhada`, `Recarga da configuração`, `Renderização iluminada`/`Caminho legado`; manter glossário.
- **Critérios:** termos novos; sem `_Avoid_: z-buffer`; `width`/`height`; sem detalhes de implementação; sem contradizer código.
- **Verificação:** `grep` dos termos; inspeção contra `Canvas.h`, `Lighting.h`, `Renderer.h`.
- **Dependências:** T1.
- **Arquivos:** `CONTEXT.md`.

### T4 — Validação de links/comandos/contagem (S)

- **Descrição:** tester valida todos os links/paths, comandos e `ctest -N`; confere que `README` não cita 1ª VA/branco como atual e que não há presets.
- **Critérios:** todos os links existem; 32 testes; comandos válidos; `git diff -- src` vazio; `camera` local não incluída.
- **Verificação:** script de links; `ctest -N`; `make build/test/smoke`; `git diff --check`.
- **Dependências:** T2, T3.
- **Arquivos:** nenhum.

### T5 — Review (S)

- **Descrição:** reviewer confere escopo docs-only, precisão técnica, links, tom, critérios e ausência de mudanças indevidas.
- **Critérios:** veredito `APROVADO`; achados corrigidos; sem `src/`/testes/CMake.
- **Verificação:** revisão do diff + comandos.
- **Dependências:** T4.

### T6 — Commit na `dev`, push e PR `dev` → `main` (M)

- **Descrição:** um commit docs-only em `dev`; push; PR `dev` → `main` com todo o trabalho da 2ª VA (#26–#28); CI verde.
- **Commit:** `docs: atualiza README e CONTEXT da 2ª VA (#28)`.
- **Incluídos:** `README.md`, `CONTEXT.md`, `tasks/m28-plan.md`.
- **Fora:** `src/`, `tests/`, `CMakeLists.txt`, `camera/`, `lighting/`, `docs/adr/`, `AGENTS.md`.
- **PR:** corpo com resumo da 2ª VA, 32/32, smoke, comandos, links para PRs #33/#34 e issues #26/#27; `Closes #28`; sem merge automático.
- **Critérios:** commit único; push; PR aberto; CI `build-and-test` verde.
- **Dependências:** T5.

## 6. Checkpoints

| Checkpoint | Critérios |
|---|---|
| CP0 — auditoria | gaps/links/contagem mapeados; 32 testes; `camera` local ignorada. |
| CP1 — README | seções 1–12; sem 1ª VA/branco como atual; comandos/links; 32 testes. |
| CP2 — CONTEXT | termos completos; sem `_Avoid_: z-buffer`; `Canvas` width/height; sem detalhes de implementação. |
| CP3 — validação | links válidos; `make build/test/smoke`; 32/32; `git diff -- src` vazio. |
| CP4 — review | `APROVADO`; sem mudanças fora de docs. |
| CP5 — PR | commit em `dev`; push; PR `dev`→`main`; CI verde. |

## 7. Riscos e mitigações

| Risco | Mitigação |
|---|---|
| Link quebrado (ADR 0008/0010 ainda não existe) | Linkar só 0001–0007/0009; registrar 0008/0010 como #29. |
| README prometer comportamento inexistente | Conferir `src/`/testes; documentar limitações (sem clipping/backface culling/múltiplas luzes). |
| Contagem 23 vs 32 | `ctest -N` = 32; listar/agrupar os 32 nomes. |
| AGENTS “não versionado” | `git ls-files` confirma versionado; linkar e registrar #30 para a verificação final. |
| Alteração local de `camera/camera.txt` | Não stage/commit; docs descrevem o HEAD (`d=5`, `hx=hy=2`). |
| PR `dev`→`main` grande | Corpo resume #26–#28; CI valida; sem merge automático. |
| Sobreposição #29/#30 | Não criar ADRs/AGENTS; apenas links/registro de pendência. |
| Termo `Recarga da cena` vs `Recarga da configuração` | Manter o termo alinhado aos requisitos e citar “recarga da cena” na descrição. |

## 8. Questões abertas

Nenhuma bloqueante. Decisões: manter `Recarga da configuração` como termo canônico (com “recarga da cena” na descrição); linkar `AGENTS.md` e registrar #30; ADRs 0008/0010 pendentes #29.

## 9. Definition of Done da #28

- [ ] README atualizado para a 2ª VA; sem 1ª VA/branco como comportamento atual.
- [ ] README documenta CLI de 3 arquivos, controles/`R`, câmera, iluminação, pipeline, equações, testes, critérios, estrutura e links.
- [ ] CONTEXT cobre os termos da 2ª VA; `Canvas` width/height; `Scan line` sem `_Avoid_: z-buffer`; sem detalhes de implementação.
- [ ] Todos os links válidos; 32 testes; sem presets por malha.
- [ ] `git diff -- src` vazio; nenhuma alteração em `camera/`, `lighting/`, testes/CMake/ADRs.
- [ ] Reviewer `APROVADO`.
- [ ] Commit único em `dev`; push; PR `dev`→`main` com #26–#28; CI verde.
- [ ] Relatório final no formato do AGENTS.

## 10. Escopo do commit e PR

- **Branch:** `dev` (atual).
- **Commit:** `docs: atualiza README e CONTEXT da 2ª VA (#28)`.
- **Arquivos:** `README.md`, `CONTEXT.md`, `tasks/m28-plan.md`.
- **PR:** `dev` → `main`, título `docs: 2ª VA — README, CONTEXT e entrega da aplicação (#26–#28)`; corpo com resumo, testes (32/32), smoke, links #33/#34 e issues #26/#27; `Closes #28`; sem merge automático.

## 11. Handoff

1. **Tester:** T1 (auditoria) e T4 (links/comandos/32).
2. **Coder:** T2/T3 (README/CONTEXT), sem tocar código.
3. **Reviewer:** T5.
4. **Responsável:** T6 (commit/push/PR/CI).
