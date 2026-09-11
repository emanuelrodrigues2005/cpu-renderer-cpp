# Diretrizes para agentes — cpu-renderer-cpp

## Visão geral

Este repositório implementa um pipeline gráfico simplificado em C++ (CPU), reproduzindo as etapas de uma GPU: leitura de malha BYU, câmera virtual, transformação mundo→vista, projeção em perspectiva, normalização, coordenadas de tela e rasterização *scan line*.

## Documentos de referência (leia antes de implementar)

- `docs/requisitos.md` — Levantamento de Requisitos do projeto (RF01–RF08, RNF01–RNF03, fases e critérios de aceitação).
- `docs/especificacao-1va.md` — transcrição da especificação da 1ª VA distribuída pelo professor.
- `docs/adr/` — decisões arquiteturais já tomadas; não as contradiga sem propor uma nova ADR.
- `CONTEXT.md` — glossário do domínio; use os termos canônicos.
- O PDF original do professor não é versionado (material da disciplina).

## Mapa dos módulos

| Módulo | Responsabilidade |
|---|---|
| `src/Vec3` | matemática vetorial (RF02) |
| `src/Mesh` | leitura e armazenamento de malhas BYU (RF01) |
| `src/Camera` | parâmetros, base U/V/N e mundo→vista (RF03/RF04) |
| `src/Projection` | perspectiva, normalização e coordenadas de tela (RF05/RF06) |
| `src/Rasterizer` | scan line (RF07) |
| `src/Renderer` | integração do pipeline (RF08) |
| `src/Canvas.h` | contrato de desenho (`clear`, `drawPixel`) |
| `src/WindowSDL` | janela e pixels via SDL2 (RF08) |
| `src/MemoryCanvas` | canvas em memória (testes e dump) |
| `src/BmpWriter` | escrita de BMP 24-bit com a biblioteca padrão |

## Comandos

```sh
make build     # configure + build
make run ARGS="models/maca.byu camera/camera.txt"
make smoke     # verificação headless (exit 0)
make test      # ctest
make clean
```

Equivalentes com CMake puro:

```sh
cmake -S . -B build
cmake --build build
cmake --build build --target run      # também: smoke, check
./build/cpu-renderer models/piramide.byu camera/camera.txt
SDL_VIDEODRIVER=dummy ./build/cpu-renderer --smoke
ctest --test-dir build --output-on-failure
```

## Regras de código

- C++17; apenas a biblioteca padrão. SDL2 somente para janela e `drawPixel` (`SDL_RenderDrawPoint`); **nunca** usar texturas, primitivas de linha/retângulo/triângulo, OpenGL ou qualquer motor gráfico.
- Toda a matemática de transformação e a rasterização são manuais.
- Índices de triângulos do arquivo BYU são 1-based; converta para 0-based no carregamento e valide os limites.
- Mantenha a separação de responsabilidades: cada módulo tem uma função clara e não conhece detalhes dos demais além do seu header.
- Siga o `.clang-format` do repositório.
- Sem comentários no meio do código: nomes e funções claras explicam a intenção. Um comentário só entra quando o código não expressa o motivo sozinho (fórmula, convenção de borda, restrição externa) — e deve ser reportado ao final, com o motivo.
- Prefira mudanças pequenas e incrementais, validando cada etapa antes da próxima.

## Fluxo de trabalho (obrigatório)

1. Trabalhe em **uma issue por vez**, seguindo a ordem de dependências descrita no corpo dela.
2. **Nunca** execute `git add`, `git commit`, `git push`, `git checkout -b`, `git merge` ou qualquer escrita via `gh` (issues, PRs, comentários). Os commits são **manuais**, feitos pelo responsável do projeto.
3. Ao concluir uma issue: rode build e testes, garanta zero warnings, atualize `CONTEXT.md`/ADRs se houver decisão nova e reporte (a) arquivos criados/alterados, (b) comandos executados, (c) resultado da verificação e (d) **cada comentário adicionado no código, com o motivo**, para o responsável revisar e corrigir se necessário.
4. Não avance para a issue seguinte sem o commit manual da anterior.

## Definition of Done (por issue)

- [ ] `cmake --build build` sem warnings.
- [ ] `ctest` verde (testes relevantes para a mudança).
- [ ] Critérios de aceitação da issue atendidos.
- [ ] `CONTEXT.md` e ADRs atualizados quando a mudança cria/afeta um termo ou decisão.
- [ ] Relatório final com evidências; nenhum commit/push realizado.
