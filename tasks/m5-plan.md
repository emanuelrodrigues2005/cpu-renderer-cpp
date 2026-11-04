# Plano de implementação — Milestone 5: 2ª VA — Z-buffer e Phong shading

## Objetivo

Implementar em `feat/m5` (a partir de `dev`) os módulos de profundidade e iluminação da milestone 5:
`Projection` com `invZ`, `DepthBuffer`, `Phong`, `ShadedRasterizer` e a sobrecarga iluminada de `renderMesh`.
A integração no `main`/CLI/recarga `R` fica para #26 (M6).

## Arquitetura aprovada pelo architect

- Extensão aditiva: `Rasterizer` e `renderMesh` legado intactos; `Points`/`Wireframe` sem profundidade/iluminação.
- `ProjectedVertex{screen, viewZ, invZ}`; `projectToScreenWithDepth`; `projectToScreen` wrapper.
- `DepthBuffer` por chamada, `clear=0`, `testAndSet(invZ)` com `invZ > atual`, empate mantém o primeiro; `at` neutro fora dos limites; dimensões negativas → 0; `invZ` não finito nunca vence.
- `phongColor(P,N,eye,Lighting)` puro; two-sided; normal nula → só ambiente; especular sem `Od`; clamp/arredondamento sem gamma.
- `ShadedVertex{screen, invZ, world, normal}`; scan line; `invZ` linear em tela; atributos ponderados por `invZ`; normal renormalizada/fallback; testar profundidade antes de iluminar; rejeitar triângulo com qualquer `invZ <= 0`/não finito.
- `Renderer`: normais 1×/chamada, projeção com profundidade, `DepthBuffer` por chamada, `fillTriangleShaded`; legado preservado.
- ADR-0007 em #23 e ADR-0009 em #24; #29 fica com 0008/0010. Atualizar referências em `docs/requisitos-2va.md`.
- `CONTEXT.md` recebe os termos da M5.

## Correções obrigatórias em relação à referência `wip/2va-completa`

1. `Projection`: calcular em cópia local e só atribuir no sucesso; falha não muta a saída.
2. `DepthBuffer`: dimensões negativas → 0; `testAndSet` rejeita `NaN`/`±inf`.
3. `ShadedRasterizer`: rejeitar no início triângulo com qualquer `invZ <= 0`/não finito.
4. Testes extras: cobertura equivalente ao legado, empate de profundidade, clip, buffer não quadrado, normal nula/fallback, `P==Pl`, `P==C`, `N` não unitária, `eta` fracionária, `eye=camera.c`, `Points`/`Wireframe` iguais ao legado.

## Tarefas

### #22 — Projection com profundidade + DepthBuffer
- Arquivos: `src/pipeline/Projection.{h,cpp}`, `src/pipeline/DepthBuffer.{h,cpp}`, `tests/test_projection.cpp`, `tests/test_depth_buffer.cpp`, `CMakeLists.txt`, `CONTEXT.md`.
- Testes: `invZ=1/viewZ`; `viewZ<=0` rejeitado; falha não muta; wrapper; `DepthBuffer` clear/maior vence/empate/fora dos limites/dimensões negativas/NaN/±inf/não quadrado.
- Commit: `feat(pipeline): adiciona profundidade de projeção e z-buffer (#22)`.

### #23 — Equação de Phong
- Arquivos: `src/shading/Phong.{h,cpp}`, `tests/test_phong.cpp`, `CMakeLists.txt`, `docs/adr/0007-modelo-iluminacao-phong.md`, `CONTEXT.md`, `docs/requisitos-2va.md`.
- Testes: ambiente/difusa/especular à mão; especular sem `Od`; two-sided; `R·V<0`; `eta`; normal nula/não unitária; `P==Pl`; `P==C`; clamp/arredondamento; entradas não finitas.
- Commit: `feat(shading): implementa a equação de iluminação de Phong (#23)`.

### #24 — ShadedRasterizer
- Arquivos: `src/pipeline/ShadedRasterizer.{h,cpp}`, `tests/test_shaded_rasterizer.cpp`, `CMakeLists.txt`, `docs/adr/0009-z-buffer-interpolacao-perspectiva.md`, `CONTEXT.md`, `docs/requisitos-2va.md`.
- Testes: cobertura igual ao legado; `invZ` linear (`0.375`); atributos perspectiva-corretos; normal renormalizada/fallback; ordem/empate; clip; degenerados; `invZ<=0`/NaN/inf; buffer não quadrado.
- Commit: `feat(rasterizer): scan line perspectiva-correta com z-buffer (#24)`.

### #25 — Renderer iluminado
- Arquivos: `src/pipeline/Renderer.{h,cpp}`, `tests/test_renderer_phong.cpp`, `CMakeLists.txt`, `CONTEXT.md`.
- Testes: cor ambiente/difusa conhecida; z-buffer entre triângulos; depth recriado por chamada; `Points`/`Wireframe` iguais ao legado; caminho legado branco; `eye=camera.c`; vértice atrás; 6 modelos com >1 cor; canvas pequeno.
- Commit: `feat(renderer): integra Phong shading por pixel (#25)`.

## Validação visual

- `make dev`: janela abre e mostra a pirâmide branca (caminho legado; M5 não integra a CLI).
- Harness temporário em `/tmp/opencode/m5-visual/render_phong.cpp`, fora do repo, gera BMPs 800×600 dos 6 modelos com `camera/camera.txt` + `lighting/lighting.txt`.
- Critério: cada modelo > 500 pixels não pretos e ≥ 8 cores distintas; inspecionar `maca`, `vaso` e `calice2`.
- `make smoke` exit 0; `--dump` continua legado (esperado até #26).

## Definition of Done

- [ ] Build sem warnings; `ctest` 23/23; `make smoke` exit 0.
- [ ] #22–#25 com critérios atendidos e evidência.
- [ ] `Rasterizer`, `main.cpp`, `Lighting`, `Mesh`, `Normals`, `Canvas` não alterados (exceto CMake).
- [ ] ADR-0007/0009 criadas; 0008/0010 não.
- [ ] `CONTEXT.md` e `docs/requisitos-2va.md` consistentes.
- [ ] Harness visual e `make dev` executados.
- [ ] Nenhum BMP/harness versionado.
- [ ] PR `feat/m5` → `dev` aberto, sem merge.
