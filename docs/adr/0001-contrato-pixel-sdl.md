# Contrato de desenho: SDL apenas para janela e `SDL_RenderDrawPoint`

O enunciado permite uma única função gráfica: pintar um pixel colorido. Para não deixar margem de interpretação, a camada SDL (`WindowSDL`) expõe somente `clear` e `drawPixel` e usa exclusivamente `SDL_CreateWindow`, `SDL_CreateRenderer`, `SDL_RenderClear`, `SDL_SetRenderDrawColor`, `SDL_RenderDrawPoint` e `SDL_RenderPresent`; texturas, primitivas de linha/retângulo e aceleração de geometria ficam de fora. A alternativa mais rápida (enviar um framebuffer completo como textura) foi descartada por poder ser questionada como função gráfica além da permitida; o custo de desempenho é aceitável porque o redesenho é event-driven.

## Consequências

- O `Rasterizer` depende apenas do contrato `Canvas`, nunca de SDL.
- O modo `--dump` e os testes usam `MemoryCanvas`, sem SDL.
