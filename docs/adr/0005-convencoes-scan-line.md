# Scan line: varredura inclusiva, arredondamento e horizontais

A rasterização preenche cada triângulo por varredura: ordena os vértices por `y`, percorre as linhas de `yTop` a `yBottom` inclusive, calcula as interseções com as arestas não horizontais e pinta de `lround(x_min)` a `lround(x_max)` inclusive. A inclusão das duas pontas repinta pixels compartilhados por triângulos vizinhos, o que é inofensivo com cor sólida e garante que os vértices extremos recebam pixel. Arestas horizontais não entram no cálculo (as outras duas sempre cobrem a linha); triângulos de área zero (colineares ou totalmente horizontais) são descartados, então não há pixels espúrios nem divisão por zero. Sem backface culling: todo triângulo de área não nula é preenchido, independente do sentido dos vértices. A varredura é limitada à janela do `Canvas` antes de pintar, para que coordenadas muito fora não gerem laços enormes.

## Consequências

- `Canvas::drawPixel` continua sendo a única função gráfica chamada.
- Bordas duplicadas entre triângulos adjacentes não são problema: cor sólida, sem blending nem z-buffer.
