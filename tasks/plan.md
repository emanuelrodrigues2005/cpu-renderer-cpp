# Plano de implementação — Milestone 4: 2ª VA — Fundação de iluminação

## Visão geral

Fechar as issues #17–#21 em `dev`, uma por commit, com testes headless, build sem warnings e validação manual pelos comandos `make`. O working tree já contém a 2ª VA inteira (milestones 4–6) não commitada; todo esse trabalho será congelado no branch local `wip/2va-completa` antes de reconstruir a milestone 4. As milestones 5–6 ficam preservadas para uma entrega posterior.

**Tracker:** GitHub Issues #17–#21 (milestone 4). Este documento é o índice ordenado; não há duplicação em `tasks/todo.md`.

## Decisões aprovadas

- **Escopo:** opção C — PR somente da milestone 4; milestones 5–6 preservadas em `wip/2va-completa`, não publicadas.
- **Parser de iluminação:** todas as mensagens de erro citam o campo individual e o caminho.
- **Testes verdes de primeira:** aceitar cobertura nova + evidência sem forçar um red artificial. Incluir `Normal de face` e `Normal de vértice` no `CONTEXT.md` em #21.
- **Git:** commits pequenos por issue, em `dev`; PR `dev → main` sem merge. Nenhum commit em `main`.

## Estratégia git

1. Snapshot do working tree em `/tmp/opencode/2va-wip-<timestamp>.tar.gz`.
2. Criar `wip/2va-completa`, commitar todo o trabalho atual (M4+M5+M6) localmente.
3. Voltar para `dev`, avançar `dev` até `main` por fast-forward (main só tem merges à frente; conteúdo idêntico).
4. Reconstruir a milestone 4 em `dev` importando apenas os arquivos de cada issue de `wip/2va-completa`, editando à mão os arquivos mistos (`CMakeLists.txt`, `CONTEXT.md`).
5. Cada commit deve compilar e passar `ctest` em build limpo.
6. Ao final, push de `dev` e PR para `main` (requer autenticação `gh`).

## Lista de tarefas

### Fase 1 — Fundação documental

- [x] **#17 — docs: versionar e restaurar a documentação da 1ª VA**
  - Importar `.gitignore`, ADRs 0001–0006, `docs/requisitos-1va.md`, `docs/especificacao-1va.md`.
  - Corrigir `docs/adr/0005-convencoes-scan-line.md` para ficar idêntica ao histórico (remover nota de status; a ADR-0009 já registra a superação).
  - Verificar `git check-ignore -v docs/ AGENTS.md` (exit 1, sem saída), diffs idênticos, ausência de PDFs/binários.
  - Commit: `docs: versiona e restaura a documentação da 1ª VA (#17)`.

- [x] **#18 — docs: especificação e requisitos da 2ª VA**
  - Importar `docs/especificacao-2va.md` e `docs/requisitos-2va.md`.
  - Verificar RF09–RF16, RNF04–RNF06, parâmetros do PDF, z-buffer/scan line/recarga e rastreabilidade.
  - Commit: `docs: adiciona especificação e requisitos da 2ª VA (#18)`.

### Checkpoint 1 — documentação

- [x] `git check-ignore` OK; docs restaurados; nenhum binário; reviewer aprovou.

### Fase 2 — Base matemática

- [x] **#19 — feat(math): cor RGB e operações vetoriais para iluminação**
  - Importar `src/math/Color.{h,cpp}`, `src/math/Vec3.{h,cpp}`, `tests/test_color.cpp`, `tests/test_vec3.cpp`.
  - Adicionar `test_color` ao CMake e ao target `check`.
  - Tester caracteriza o comportamento; cobertura extra opcional para arredondamento/clamp.
  - Commit: `feat(math): adiciona cor RGB e operações vetoriais de iluminação (#19)`.

### Fase 3 — Iluminação e normais

- [x] **#20 — feat(lighting): arquivo de iluminação, validação e exemplos**
  - Importar `src/shading/Lighting.{h,cpp}`, `lighting/lighting.txt`, `lighting/lighting-pdf.txt`, `tests/test_lighting.cpp`, fixtures.
  - Tester escreve testes red: carregar `lighting/lighting.txt`; erro de não-finito cita campo e caminho; erros de intervalo citam campo e caminho; erros estruturais citam campo e caminho.
  - Coder ajusta `Lighting.cpp` para validar por campo, mantendo atomicidade e mensagens PT-BR.
  - Tester revalida; reviewer revisa parser, fixtures e aceitação.
  - Commit: `feat(lighting): carrega e valida parâmetros de iluminação (#20)`.

- [x] **#21 — feat(mesh): normais de face e de vértice**
  - Importar `src/mesh/Normals.{h,cpp}`, `tests/test_normals.cpp`; adicionar `Normals.cpp` ao binário e `test_normals` ao CMake/check.
  - Tester adiciona: tetraedro com normais unitárias, média não ponderada por área, normal de vértice nula finita, face degenerada não contribui.
  - Coder só altera se houver red real; caso contrário registra no-op.
  - Atualizar `CONTEXT.md` com `Normal de face` e `Normal de vértice`.
  - Tester revalida; reviewer revisa matemática, casos degenerados e aceitação.
  - Commit: `feat(mesh): calcula normais de face e de vértice (#21)`.

### Checkpoint 2 — código

- [x] Build limpo sem warnings; `ctest` 19/19; `make smoke` exit 0; `test_color`, `test_lighting`, `test_normals` verdes.

### Fase 4 — Validação final e PR

- [x] `make clean && make build`, `make test`, `make smoke`.
- [x] `make dev` com janela (X11 disponível) e captura visual; `--dump` dos 6 modelos; `--info` dos 6 modelos.
- [x] `clang-format --dry-run --Werror` nos arquivos alterados (se disponível); `git diff --check`.
- [x] Reviewer final e tester final em paralelo; corrigir achados via coder.
- [x] Push `dev`; abrir PR `dev → main` com descrição detalhada e passo a passo manual; não fazer merge.

## Critérios de aceitação globais

- [x] Issues #17–#21 com critérios atendidos um a um.
- [x] 5 commits, um por issue, cada um buildável e com testes verdes.
- [x] Nenhum arquivo de M5/M6 commitado no PR da M4.
- [x] `wip/2va-completa` preserva integralmente o trabalho das milestones 5–6.
- [x] `CONTEXT.md` atualizado com os dois termos de normais.
- [x] Nenhum PDF/binário versionado.
- [x] PR aberto, sem merge, com validação manual reproduzível.

## Riscos e mitigações

| Risco | Impacto | Mitigação |
|---|---|---|
| Perder o trabalho não commitado de M5/M6 | Alto | tar de segurança + branch local `wip/2va-completa`; conferir `git diff wip --stat` vazio |
| `git add -A` capturar lixo | Baixo | revisar `git status` antes do commit; build/dumps/BMP já ignorados |
| CMakeLists parcial quebrar build | Médio | build limpo em `/tmp/opencode/build-m4` a cada commit |
| ADR-0005 não ficar byte-idêntica | Médio | `git show <commit>:... | diff -` (histórico tem 1144 bytes) |
| Mensagens de erro acopladas a `nan`/`inf` | Baixo | testar campo + caminho, não a frase exata |
| `gh` sem autenticação | Alto | pedir autenticação ao usuário antes do push/PR; salvar corpo do PR em `/tmp/opencode/pr-m4.md` |

## Resultado final

- PR aberto: https://github.com/emanuelrodrigues2005/cpu-renderer-cpp/pull/31
- Estado: OPEN, mergeable, sem merge.
- Commits: 7 (5 issues + 2 polish pós-review).
- CI: build-and-test pass nos dois workflows (push e PR).
- Testes: 19/19; `make smoke` exit 0; 6 modelos com `--info`/`--dump`.
- `wip/2va-completa`: preservado localmente com as milestones 5-6.
