Tracker: GitHub issue #29; este documento é o índice ordenado; não há duplicação em `tasks/todo.md`.

# Plano de implementação — Issue #29: ADRs da 2ª VA

## 1. Visão geral e objetivo

Registrar as decisões arquiteturais da 2ª VA em ADRs completas: atualizar a ADR-0007 (modelo de iluminação e escopo somente Phong), criar a ADR-0008 (normais de face/vértice e two-sided), atualizar a ADR-0009 (z-buffer e interpolação perspectiva-correta) e criar a ADR-0010 (câmera e iluminação em arquivos separados, CLI e recarga). A ADR-0005 passa a indicar que foi superada pela ADR-0009 no que se refere à ausência de z-buffer. As decisões devem corresponder ao código, ao README e aos requisitos, e os links entre ADRs e documentos devem funcionar.

A issue é **docs-only**: nenhum `src/`, teste, CMake, câmera, iluminação ou AGENTS é alterado. O trabalho é feito diretamente na `dev` (a pedido do responsável), atualizando o PR #35 (`dev` → `main`); não há PR intermediário para `dev`.

## 2. Não-objetivos

- Não alterar comportamento, código, testes ou CMake.
- Não criar/alterar `AGENTS.md` (issue #30) nem a verificação final de links globais (issue #30).
- Não alterar `README.md` (a atualização final é #30); a #29 atualiza apenas ADRs e os docs de requisitos/especificação.
- Não reescrever ADRs 0001–0006 além da marcação de status da 0005.
- Não incluir a alteração local de `camera/camera.txt` (inexistente no momento; confirmar `git diff`).

## 3. Decisões de documentação

As ADRs 0007–0010 devem ter as seções **Contexto**, **Decisão**, **Alternativas** e **Consequências**, com links relativos entre si e para os documentos do projeto. A ADR-0005 é exceção: preserva o corpo e recebe apenas o status de superada e o ajuste da consequência que menciona a ausência de z-buffer.

### ADR-0007 — Modelo de iluminação de Phong

- **Contexto:** 2ª VA; modelo local com uma única luz pontual em coordenadas de mundo; slides/PDF; escopo somente Phong shading.
- **Decisão:** equações `L`, `V`, `R`; `Ia`, `Id`, `Is`; `Iamb/Il [0,255]`; `Ka/Ks/Kd/Od [0,1]`; `eta >= 1`; especular sem `Od`; regras two-sided; clamp por canal `[0,255]` com arredondamento e sem gamma; normal nula/direção degenerada → ambiente; uma luz e um material.
- **Alternativas:** flat shading (sem interpolação de normais), Gouraud (cores por vértice; destaque especular perde qualidade), Blinn-Phong (modelo diferente do pedido), múltiplas luzes/atenuação/sombras (fora do escopo).
- **Consequências:** custo por pixel; iluminação suave; sem gamma; uma luz/material; remete à ADR-0008 (normais/two-sided) e à ADR-0010 (configuração).
- Manter a fórmula e os casos especiais atuais; adicionar links para ADR-0008/0010.

### ADR-0008 — Normais de face e de vértice e two-sided

- **Contexto:** o BYU não traz normais; a tonalização de Phong precisa de normais suaves por vértice; malhas abertas exigem two-sided.
- **Decisão:**
  - normal de face = `normalize((b−a) × (c−a))`; faces degeneradas/nulas não contribuem;
  - normal de vértice = soma das normais unitárias das faces incidentes, depois normalizada; média **não ponderada**; vértice sem face incidente = vetor nulo; normais não finitas rejeitadas/substituídas por zero; resultado sempre finito;
  - normal interpolada é renormalizada; se nula/não finita, usa-se a normal da face; se essa também for nula, cai no caso de normal nula (só ambiente);
  - two-sided: inverter `N` se `N·L < 0` e `V·N < 0`; se ainda `N·L < 0`, sem difusa/especular; se `R·V < 0`, sem especular.
- **Alternativas:** usar normais do BYU (não existem/ambíguas), ponderação por área/ângulo (mais complexa; não exigida), sem two-sided (faces invertidas pretas), flat shading (fora do escopo).
- **Consequências:** iluminação suave em vértices compartilhados; arestas duras exigem vértices duplicados; faces degeneradas ignoradas; malhas abertas iluminadas nos dois lados; testes `normals`, `phong`, `renderer_phong`.
- Links: ADR-0007, `CONTEXT.md`, `src/mesh/Normals`, `tests/test_normals.cpp`.

### ADR-0009 — Z-buffer e interpolação perspectiva-correta

- **Contexto:** visibilidade e atributos corretos no scan line; a ADR-0005 cobria a varredura sem z-buffer.
- **Decisão:**
  - `invZ = 1/z`; buffer limpo com `0`; `invZ` maior vence; empate mantém o primeiro; `invZ` não finito nunca vence; consultas/escritas fora dos limites neutras;
  - atributos (mundo/normal) interpolados com pesos baricêntricos multiplicados por `invZ` e divididos pelo `invZ` final; `z` nunca linear em tela; normal renormalizada/fallback;
  - sem ordenação por baricentro; sem backface culling; sem clipping de plano próximo; triângulo com qualquer `invZ <= 0`/não finito é descartado;
  - preservar as salvaguardas atuais: rejeição de coordenadas normalizadas acima de `1e5` na projeção, dimensões negativas → `0` no `DepthBuffer`, clamp de pesos baricêntricos negativos e `invZ` não finito nunca vence;
  - `Points`/`Wireframe` sem profundidade.
- **Alternativas:** painter's/depth sort (incorreto para interseções), `z` linear (distorção), profundidade por triângulo (grosseira), clipping de plano próximo (fora do escopo).
- **Consequências:** ordem independente; interpolação correta; limitação em zoom extremo; buffer ~1,9 MB em 800×600; a ADR-0005 é superada no aspecto de ausência de z-buffer, mas as convenções de varredura/arredondamento/horizontais continuam válidas.
- Links: ADR-0005, ADR-0007, `src/pipeline/DepthBuffer`, `src/pipeline/ShadedRasterizer`, `tests/test_shaded_rasterizer.cpp`.

### ADR-0010 — Câmera e iluminação em arquivos separados

- **Contexto:** parâmetros de câmera e iluminação vêm de arquivos texto; o PDF usa um exemplo de iluminação; o projeto tem câmera compartilhada (ADR-0006) e CLI de três posicionais.
- **Decisão:**
  - dois arquivos: `camera/camera.txt` e `lighting/lighting.txt`; padrões do projeto;
  - terceiro posicional opcional `[modelo.byu] [camera.txt] [lighting.txt]`;
  - `lighting/lighting.txt` é calibrado para os modelos/câmera; `lighting/lighting-pdf.txt` preserva o exemplo exato do PDF e **não** é padrão;
  - `R` recarrega câmera e iluminação pelos caminhos correntes; cada fonte é atômica; erro preserva o último valor válido daquela fonte, reporta no `stderr` e não invalida a outra; redesenha se ao menos uma recarregar;
  - `1`–`6` troca só a malha; câmera/iluminação compartilhadas.
- **Alternativas:** um arquivo único (menos separação), presets por malha (contraria a câmera compartilhada/ADR-0006), recarregar só a câmera (iluminação antiga), usar o exemplo do PDF como padrão (calibração inadequada).
- **Consequências:** dois arquivos para editar; `R` recarrega ambos; atomicidade por fonte; luz padrão calibrada e exemplo do PDF separado; testes `lighting_policy`, `lighting_padrao`, `lighting_sucesso`, `lighting_*`, `camera_padrao`.
- Links: ADR-0006, ADR-0002, ADR-0007, `src/main.cpp`, `src/shading/Lighting`, `lighting/`.

### ADR-0005 — status de superada

Adicionar no topo: `Status: superada pela ADR-0009 no que se refere à ausência de z-buffer; as convenções de varredura, arredondamento e arestas horizontais continuam válidas.` Ajustar a consequência que menciona “sem z-buffer” para indicar a ADR-0009. Manter o restante do corpo.

## 4. Matriz requisito → arquivo → verificação

| Requisito #29 | Arquivo | Verificação |
|---|---|---|
| ADR-0007 com contexto/decisão/alternativas/consequências, somente Phong | `docs/adr/0007-modelo-iluminacao-phong.md` | headings + equações + `grep` flat/Gouraud |
| ADR-0008 normais/média/fallback/two-sided | `docs/adr/0008-normais-face-vertice-two-sided.md` | conteúdo vs `src/mesh/Normals.cpp`, `src/shading/Phong.cpp` |
| ADR-0009 `1/z`, teste, interpolação, sem ordenação/clipping; supera 0005 | `docs/adr/0009-z-buffer-interpolacao-perspectiva.md` | conteúdo vs `DepthBuffer`/`ShadedRasterizer`; link 0005 |
| ADR-0010 dois arquivos/CLI/`R`/atômico/luz padrão/PDF | `docs/adr/0010-camera-iluminacao-arquivos-separados.md` | conteúdo vs `src/main.cpp`, `Lighting.cpp` |
| ADR-0005 superada | `docs/adr/0005-convencoes-scan-line.md` | status + link 0009 |
| Links entre ADRs/docs | ADRs e `docs/requisitos-2va.md` | script de links |
| Decisões correspondem ao código/README | ADRs | inspeção + README |
| Nenhum `src/`/teste/CMake alterado | — | `git diff -- src tests CMakeLists.txt` vazio |

## 5. Tarefas

### T1 — Auditoria das ADRs atuais (S)

- **Descrição:** tester audita 0005/0007/0009 e referências em `docs/requisitos-2va.md`/`README.md`; lista lacunas e links.
- **Critérios:** matriz de lacunas; 0005 sem status; 0008/0010 ausentes; 0007/0009 sem seções de alternativas; referências a 0008/0010.
- **Verificação:** leitura/grep; `git diff -- src` vazio.
- **Dependências:** nenhuma.

### T2 — ADR-0007 (S)

- **Descrição:** reestruturar com Contexto/Decisão/Alternativas/Consequências; manter equações/ranges/two-sided/clamp/sem gamma; links para 0008/0010.
- **Critérios:** quatro seções; somente Phong; flat/Gouraud como alternativas/fora de escopo; decisão compatível com `Phong.cpp`; remover a frase final “ambas ficam para #29” (linha 47) e verificar com `grep -n "ficam para #29"`.
- **Verificação:** headings; revisão contra `src/shading/Phong.cpp`.
- **Dependências:** T1.

### T3 — ADR-0008 (M)

- **Descrição:** criar a ADR-0008 conforme 3.
- **Critérios:** quatro seções; cálculo/média/fallback/two-sided; alternativas/consequências; links.
- **Verificação:** revisão contra `Normals.cpp`/`Phong.cpp`; links.
- **Dependências:** T1.

### T4 — ADR-0009 (M)

- **Descrição:** reestruturar a ADR-0009 com quatro seções; cobrir `invZ`, teste, interpolação, sem ordenação/clipping, `Points`/`Wireframe`; preservar as salvaguardas (>1e5, dimensões negativas→0, pesos negativos, `invZ` não finito); link 0005.
- **Critérios:** quatro seções; decisões compatíveis com `DepthBuffer`/`ShadedRasterizer`; 0005 referenciada.
- **Verificação:** revisão; links.
- **Dependências:** T1.

### T5 — ADR-0010 (M)

- **Descrição:** criar a ADR-0010 conforme 3.
- **Critérios:** quatro seções; dois arquivos/CLI/`R`/atomicidade/luz padrão/PDF; alternativas/consequências; links.
- **Verificação:** revisão contra `src/main.cpp`/`Lighting.cpp`/`lighting/`; links.
- **Dependências:** T1.

### T6 — ADR-0005 e referências (S)

- **Descrição:** status de superada; atualizar referências em `docs/requisitos-2va.md` (RF09–RF13/RF15, seção 7 e rastreabilidade) e, apenas na seção “Decisões de implementação” de `docs/especificacao-2va.md`, referências às ADRs 0007–0010; não reescrever requisitos.
- **Critérios:** 0005 com status; referências corretas; nenhuma menção a “0008/0010 em #29” **nem** a “0008 e 0010 em #29” nos requisitos; §7 diz que 0007/0009 são **atualizadas**, não criadas.
- **Verificação:** grep; links.
- **Dependências:** T2–T5.

### T7 — Validação e review (S)

- **Descrição:** tester valida links/headings/decisões; reviewer revisa.
- **Critérios:** links 100%; quatro seções em 0007–0010; 0005 com status; decisões vs código; 32/32; `git diff -- src` vazio; `grep -n "ficam para #29" docs/adr/0007-modelo-iluminacao-phong.md` sem resultado.
- **Verificação:** script de links; `ctest -N`; `make test`/`smoke`; `git diff --check`.
- **Dependências:** T6.

### T8 — Commit/push e PR #35 (S)

- **Descrição:** commit docs-only na `dev`; push (atualiza PR #35); CI verde.
- **Commit:** `docs: adiciona ADRs da 2ª VA (#29)`.
- **Arquivos:** ADRs 0005/0007/0008/0009/0010, `docs/requisitos-2va.md`, `docs/especificacao-2va.md`, `tasks/m29-plan.md`.
- **Critérios:** commit único; push; PR #35 atualizado; CI verde.
- **Dependências:** T7.

## 6. Checkpoints

| Checkpoint | Critérios |
|---|---|
| CP0 — auditoria | lacunas/links mapeados; 32 testes; `src/` intocado. |
| CP1 — ADRs | 0007–0010 com quatro seções; 0008/0010 criadas; 0005 com status e corpo preservado. |
| CP2 — referências | docs/requisitos e especificação atualizados; links válidos. |
| CP3 — validação | 32/32; smoke 0; `git diff -- src` vazio; links 100%. |
| CP4 — review/commit | reviewer `APROVADO`; commit em `dev`; push; PR #35; CI verde. |

## 7. Riscos e mitigações

| Risco | Mitigação |
|---|---|
| ADR sem alternativas/consequências | checklist de quatro seções em 0007–0010; 0005 só status. |
| Decisão divergir do código | tester/architect comparam com `Normals`, `Phong`, `DepthBuffer`, `ShadedRasterizer`, `main`. |
| Link relativo quebrado | script de links; validar a partir de `docs/adr/` (`../../src/...`, `../../tests/...`, `../requisitos-2va.md`, `../especificacao-2va.md`). |
| Sobreposição #30 | #29 só ADRs/docs de requisitos; README final/AGENTS na #30. |
| ADR-0005 perder conteúdo | adicionar status, não reescrever a varredura. |
| Commit em `dev` | usuário autorizou a branch atual; PR #35 recebe os commits. |

## 8. Questões abertas

Nenhuma bloqueante. Decisões: manter a ADR-0005 e marcar status; atualizar 0007/0009; criar 0008/0010; referências em requisitos/especificação; README final na #30.

## 9. Definition of Done da #29

- [ ] ADR-0007/0008/0009/0010 com contexto/decisão/alternativas/consequências.
- [ ] ADR-0005 com status de superada pela 0009.
- [ ] Decisões correspondem ao código/README.
- [ ] Links entre ADRs e documentos válidos.
- [ ] Nenhuma alteração em `src/`, testes, CMake, câmera, iluminação ou AGENTS.
- [ ] 32/32; smoke 0; build sem warnings.
- [ ] Reviewer `APROVADO`; commit em `dev`; push; PR #35; CI verde.

## 10. Escopo do commit

Um commit em `dev`:

```text
docs: adiciona ADRs da 2ª VA (#29)
```

Incluídos: `tasks/m29-plan.md`, `docs/adr/0005`, `0007`, `0008`, `0009`, `0010`, `docs/requisitos-2va.md`, `docs/especificacao-2va.md`. Fora: `src/`, testes, CMake, `README.md` final, `AGENTS.md`, `camera/`, `lighting/`, build/BMPs.

## 11. Handoff

1. **Tester:** T1/T7 (auditoria/validação).
2. **Coder:** T2–T6 (ADRs/referências).
3. **Reviewer:** T7.
4. **Responsável:** T8 (commit/push/PR/CI).
