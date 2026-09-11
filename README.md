# cpu-renderer-cpp

Pipeline gráfico implementado manualmente em C++: leitura de malhas no formato BYU, câmera virtual, transformação mundo→vista, projeção em perspectiva, normalização, conversão para coordenadas de tela e rasterização de triângulos com o algoritmo *scan line*.

Projeto da disciplina **Computação Gráfica Básica (06230)** — UFRPE/DC, 1ª VA.

> Em construção: esta etapa entrega a fundação (build, janela SDL2 e contrato de desenho de pixel).

## Requisitos

- CMake >= 3.16
- Compilador com suporte a C++17
- SDL2 (`libsdl2-dev` no Ubuntu/Debian) — usada somente para criação da janela e desenho de pixel

## Build e execução

Atalhos via `Makefile`:

```sh
make build                  # configura e compila
make dev                    # compila e executa a aplicação
make run ARGS="models/maca.byu camera/camera.txt"
make smoke                  # verificação headless (exit 0)
make test                   # roda os testes (CTest)
make clean                  # remove build/
```

Equivalentes com CMake puro:

```sh
cmake -S . -B build
cmake --build build
cmake --build build --target run      # também: smoke, check
./build/cpu-renderer                  # modelo e câmera padrão (models/piramide.byu, camera/camera.txt)
./build/cpu-renderer models/maca.byu camera/camera.txt
SDL_VIDEODRIVER=dummy ./build/cpu-renderer --smoke
ctest --test-dir build --output-on-failure
```

## Controles

| Tecla | Ação |
|-------|------|
| `R` | Recarrega `camera.txt` e redesenha |
| `Esc` / `Q` | Encerra a aplicação |

## Estrutura do repositório

```
src/        código-fonte (pipeline e camada SDL)
models/     malhas BYU de exemplo
camera/     parâmetros da câmera virtual
tests/      testes automatizados (CTest)
docs/       requisitos, especificação da 1ª VA e ADRs
```

## Documentação

- `docs/requisitos.md` — levantamento de requisitos do projeto
- `docs/especificacao-1va.md` — transcrição da especificação da 1ª VA
- `CONTEXT.md` — glossário do domínio
- `docs/adr/` — decisões arquiteturais
- `AGENTS.md` — diretrizes para agentes e colaboradores
