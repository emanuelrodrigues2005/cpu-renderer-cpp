Tracker: GitHub issue #30; este documento é o índice ordenado.

# Plano de implementação — Issue #30: AGENTS.md e verificação final da documentação

## 1. Visão geral e objetivo

Finalizar a documentação do projeto: consolidar `AGENTS.md` como guia permanente em PT-BR (missão, documentos, restrições, módulos, convenções, fluxo por issue, DoD, comandos, armadilhas e formato de relatório), atualizar a seção de documentação do `README.md` para apontar para `AGENTS.md` e para as ADRs 0001–0010, garantir que `.gitignore` não ignore `docs/` nem `AGENTS.md` e executar a verificação final de links/consistência/build/testes/smoke.

A issue é **docs-only**. O trabalho é feito diretamente na `dev` (a pedido do responsável), atualizando o PR #35 (`dev` → `main`); não há PR intermediário para `dev`. Ao final, as issues #28/#29/#30 são fechadas no GitHub e o PR #35 fica pronto para o merge final, sem merge automático.

## 2. Não-objetivos

- Não alterar `src/`, testes, `CMakeLists.txt`, `Makefile`, `camera/`, `lighting/` ou comportamento.
- Não reescrever as ADRs 0001–0010; a #29 já as criou/atualizou. A #30 apenas linka/verifica.
- Não reabrir decisões da 2ª VA nem adicionar flat/Gouraud/clipping/múltiplas luzes.
- Não criar um PR intermediário para `dev`.
- Não incluir o PDF original da disciplina nem BMPs/build.

## 3. Decisões de documentação

### 3.1 AGENTS.md

Atualizar o arquivo existente (não recriar do zero) para o estado final:

- **Missão/escopo:** pipeline manual em C++ (CPU), 2ª VA concluída (Phong, normais, z-buffer, interpolação perspectiva-correta, app completo); 1ª VA como base.
- **Leia antes de codar:** README, CONTEXT, especificações/requisitos 1ª/2ª, ADRs 0001–0010, issue atual; PDF original não versionado.
- **Restrições:** C++17, stdlib + SDL2 somente janela/pixel (`SDL_RenderDrawPoint`); proibidos texturas/linhas/retângulos/primitivas de triângulo/`SDL_RenderCopy`/OpenGL/Vulkan/engines; matemática manual; contrato de `Canvas` com `clear`, `drawPixel`, `width()` e `height()` (corrigir a linha que dizia “único `drawPixel`”); `Points`/`Wireframe` sem iluminação/profundidade; somente Phong; sem clipping/backface culling/múltiplas luzes/sombras/texturas/transparência sem issue.
- **Mapa dos módulos:** manter a tabela atual (math/mesh/pipeline/shading/canvas/main) e conferir contra `src/`.
- **Convenções:** coordenadas; cor; normais/two-sided; profundidade/z-buffer/interpolação; configuração/recarga (dois arquivos, CLI de 3 posicionais, `R` ambos, atomicidade, `--dump`/`--smoke` Phong, `1–6` só malha).
- **Fluxo por issue:** ler issue/docs; baseline `make test`; menor mudança; testes headless; build/test/smoke; manual; atualizar CONTEXT/ADR; relatório. **Não** commitar/pushar/gh write sem autorização explícita do responsável; quando autorizado, registrar no relatório.
- **DoD:** build sem warnings; `ctest` verde (32/32 no estado atual); smoke 0; critérios; testes; CONTEXT/ADR quando aplicável; autorização; relatório.
- **Comandos:** `make build/dev/run/smoke/test/clean`, CMake/CTest, `--dump`, `--smoke`, `--info`, os 6 modelos, `R`.
- **Armadilhas:** manter a tabela atual (z linear, V da câmera, especular com Od, normais, fallback, winding, invZ, ordenação, SDL, R, troca de malha, flat/Gouraud, commit/push sem autorização).
- **Relatório final:** itens 1–8; no item 8, substituir “nenhum commit/push/gh write” por “operações de git/GitHub realizadas conforme autorização (ou nenhuma, se não autorizado)”.
- **Links:** README, CONTEXT, docs 1ª/2ª, ADRs 0001–0010; não criar link quebrado; ADRs 0008/0010 agora existem.

### 3.2 README.md — seção Documentação

- Incluir as ADRs [0008](../docs/adr/0008-normais-face-vertice-two-sided.md) e [0010](../docs/adr/0010-camera-iluminacao-arquivos-separados.md) na lista de ADRs versionadas (0001–0010).
- Remover a frase “As ADRs 0008 e 0010 pertencem à issue #29 e ainda não existem”; substituir por “ADRs 0001–0010 versionadas”.
- Manter o link para `AGENTS.md` (já existe) e para especificações/requisitos 1ª/2ª.
- Não reescrever outras seções do README (já atualizadas na #28).

### 3.3 `.gitignore` e verificação final

- `git check-ignore docs/ AGENTS.md` deve ser vazio; não adicionar nenhuma entrada.
- Verificar `git ls-files AGENTS.md docs/adr` e a existência de todos os links Markdown do README/CONTEXT/docs.
- `grep` por referências obsoletas: “0008/0010 em #29”, “0008 e 0010 em #29”, “AGENTS.md … não versionado”, “23 suítes”, “preenchimento branco” como comportamento atual (“branco” só no caminho legado/depuração), “presets de câmera” no README.
- Build/test/smoke: 32/32; `make smoke` 0; build sem warnings.

## 4. Matriz requisito → arquivo → verificação

| Requisito #30 | Arquivo | Verificação |
|---|---|---|
| Missão/escopo | `AGENTS.md` | seção Missão |
| Documentos de referência | `AGENTS.md` | tabela “Leia antes” + links |
| Restrições C++17/stdlib/SDL2 pixel/matemática manual | `AGENTS.md` | seção Restrições |
| Mapa dos módulos | `AGENTS.md` | tabela vs `src/` |
| Convenções coordenadas/cor/normais/profundidade/z-buffer/two-sided | `AGENTS.md` | seções de convenções |
| Fluxo por issue/DoD/comandos/armadilhas/relatório | `AGENTS.md` | seções correspondentes |
| `.gitignore` sem docs/AGENTS | `.gitignore` | `git check-ignore` vazio |
| README aponta AGENTS e docs 1ª/2ª | `README.md` | seção Documentação + links |
| Nenhum link quebrado/obsoleto | README/CONTEXT/docs/ADRs | script de links + grep |
| Build/test/smoke verdes | — | `make test` 32/32; `make smoke` 0 |

## 5. Tarefas

### T1 — Auditoria final (S)

- **Descrição:** tester audita AGENTS.md atual vs issue #30, README docs, `.gitignore`, links e referências obsoletas.
- **Critérios:** lista de gaps; `check-ignore` vazio; 32 testes; links.
- **Verificação:** `git check-ignore docs/ AGENTS.md`; `git ls-files`; script de links; grep.
- **Dependências:** #29 concluída.

### T2 — AGENTS.md (M)

- **Descrição:** atualizar o AGENTS.md conforme 3.1, preservando o que já está correto.
- **Critérios:** todos os tópicos da issue; contrato Canvas; fluxo/DoD/comandos/armadilhas; relatório com autorização; links.
- **Verificação:** revisão contra issue #30 e `src/`; links.
- **Dependências:** T1.

### T3 — README e `.gitignore` (S)

- **Descrição:** atualizar a seção Documentação do README (ADRs 0008/0010; lista 0001–0010; AGENTS/docs); remover a frase “a verificação final é a issue #30” do link do AGENTS; verificar `.gitignore`.
- **Critérios:** links válidos; sem “0008/0010 #29”; sem “verificação final é a issue #30”; AGENTS/docs/1ª/2ª; `git check-ignore` vazio.
- **Verificação:** script de links; `git check-ignore`.
- **Dependências:** T1.

### T4 — Verificação final (M)

- **Descrição:** rodar build/test/smoke, link checker e grep de referências obsoletas; conferir `src/` intocado.
- **Critérios:** 32/32; smoke 0; sem warnings; links 100% (script Python que resolve cada destino de README/CONTEXT/docs/adr); grep limpo; `git diff -- src tests CMakeLists camera lighting` vazio.
- **Verificação:** comandos da seção 3.3; `git diff --check`.
- **Dependências:** T2, T3.

### T5 — Review (S)

- **Descrição:** reviewer confere AGENTS/README/docs/links/escopo e dá veredito.
- **Critérios:** `APROVADO`; achados corrigidos.
- **Dependências:** T4.

### T6 — Commit/push/PR #35 e fechamento (M)

- **Descrição:** commit docs-only na `dev`; push (atualiza PR #35); CI verde; atualizar o corpo do PR #35 para incluir #29/#30 e `Closes #28, #29, #30`; fechar as issues #28/#29/#30 no GitHub (autorizado pelo responsável), registrando que o PR #35 fica aberto para o merge final e que o trabalho está em `dev`.
- **Commit:** `docs: finaliza AGENTS e documentação da 2ª VA (#30)`.
- **Arquivos:** `AGENTS.md`, `README.md`, `tasks/m30-plan.md` (somente; ADRs/requisitos/especificação já foram commitados na #29 e não entram neste commit).
- **Critérios:** commit único; push; PR #35 atualizado; CI verde; issues fechadas; PR #35 aberto para merge (sem merge automático).
- **Dependências:** T5.

## 6. Checkpoints

| Checkpoint | Critérios |
|---|---|
| CP0 — auditoria | gaps/links/ignore mapeados; 32 testes; `src/` intocado. |
| CP1 — AGENTS | todos os tópicos da #30; Canvas; fluxo/DoD/comandos/relatório. |
| CP2 — README/links | ADRs 0008/0010 linkadas; AGENTS/docs; links 100%; check-ignore vazio. |
| CP3 — validação | 32/32; smoke 0; sem warnings; grep obsoleto limpo. |
| CP4 — review/commit | reviewer `APROVADO`; commit em `dev`; push; PR #35; CI; issues fechadas. |

## 7. Riscos e mitigações

| Risco | Mitigação |
|---|---|
| AGENTS.md repetir informação desatualizada | comparar com README/CONTEXT/ADRs 0001–0010 e código. |
| Link quebrado para ADR | script de links; 0008/0010 agora existem. |
| `.gitignore` ignorar docs/AGENTS | `git check-ignore` vazio; não editar. |
| Sobreposição #29 | #30 só AGENTS/README final/verificação; ADRs já fechadas na #29. |
| Commit incluir build/BMP | `git status`/`git add` seletivo; `git diff --check`. |
| PR #35 grande | corpo atualizado com #26–#30; CI. |
| Fechar issues antes do merge | fechar após push/CI; PR #35 permanece aberto para o merge final. |

## 8. Questões abertas

Nenhuma bloqueante. Decisão: fechar #28/#29/#30 após o push/CI da #30; o PR #35 fica aberto para o merge final do responsável.

## 9. Definition of Done da #30

- [ ] AGENTS.md cobre todos os tópicos da issue.
- [ ] `.gitignore` sem `docs/`/`AGENTS.md`; `git check-ignore` vazio.
- [ ] README aponta para AGENTS, docs 1ª/2ª e ADRs 0001–0010.
- [ ] Nenhum link quebrado/referência obsoleta.
- [ ] 32/32; smoke 0; build sem warnings.
- [ ] Reviewer `APROVADO`.
- [ ] Commit em `dev`; push; PR #35 atualizado; CI verde.
- [ ] Issues #28/#29/#30 fechadas; PR #35 aberto para merge.

## 10. Escopo do commit

Um commit em `dev`:

```text
docs: finaliza AGENTS e documentação da 2ª VA (#30)
```

Incluídos: `AGENTS.md`, `README.md`, `tasks/m30-plan.md`. Fora: `src/`, testes, CMake, câmera, iluminação, ADRs (já #29), build/BMPs.

## 11. Handoff

1. **Tester:** T1/T4 (auditoria/validação).
2. **Coder:** T2/T3 (AGENTS/README).
3. **Reviewer:** T5.
4. **Responsável:** T6 (commit/push/PR/CI/fechamento).
