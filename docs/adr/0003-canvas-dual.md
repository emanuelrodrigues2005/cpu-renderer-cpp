# Canvas dual: janela SDL e canvas em memória atrás do mesmo contrato

O desenho acontece sempre contra o contrato `Canvas` (`width`, `height`, `clear` e `drawPixel`), com duas implementações: `WindowSDL` (única que toca SDL, e só janela e `SDL_RenderDrawPoint`) e `MemoryCanvas` (vetor de pixels, sem SDL). Testes e o `--dump` usam `MemoryCanvas`, então o pipeline inteiro roda headless e determinístico; a alternativa de passar tudo pelo driver *dummy* do SDL acoplaria testes e dump à SDL e esconderia regressões de pixel atrás do renderizador. A separação também mantém `Rasterizer` e `Renderer` sem dependência de plataforma.

## Consequências

- O caminho `--dump` não inicializa SDL nem abre janela.
- Os testes observam pixels por `MemoryCanvas::pixelAt`, sem telas nem mocks.
