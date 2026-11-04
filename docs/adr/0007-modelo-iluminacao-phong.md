# Modelo de iluminação de Phong e escopo somente Phong shading

## Contexto

A 2ª VA pede iluminação e tonalização de Phong. O projeto adota o modelo local de Phong com uma única luz pontual em coordenadas de mundo (`Pl`, `Il`), um único material para a cena (`Ka`, `Kd`, `Od`, `Ks`, `eta`) e a câmera compartilhada `C`. O formato BYU não traz normais: elas são calculadas pelo pipeline (ver [ADR-0008](0008-normais-face-vertice-two-sided.md)). A tonalização é por pixel, então a equação é avaliada em cada fragmento visível com posição de mundo e normal interpoladas.

O escopo da entrega é **somente Phong shading**. Flat shading e Gouraud shading aparecem nos slides como comparação, mas não são implementados. O caminho branco sem iluminação permanece como sobrecarga interna de `renderMesh` para os testes da 1ª VA, e os modos de depuração `D`/`W` usam os caminhos de pontos/arame, também sem iluminação.

## Decisão

Para um ponto `P` com normal unitária `N`, câmera em `C` e luz pontual em `Pl`:

```text
L = normalize(Pl - P)
V = normalize(C - P)
R = 2(N·L)N - L

Ia = Ka · Iamb
Id = max(N·L, 0) · Kd ∗ Od ∗ Il
Is = max(R·V, 0)^eta · Ks · Il
I  = Ia + Id + Is
```

`Iamb` e `Il` são cores RGB em `[0,255]`; `Ka`, `Kd`, `Od` e `Ks` estão em `[0,1]`; `eta >= 1`. A multiplicação `∗` é componente a componente. O especular usa `Ks` e `Il`, nunca `Od`. O `V` da iluminação é a direção do ponto para a câmera e não é o vetor `V` da base da câmera. A cor final é limitada a `[0,255]` por canal, com arredondamento na conversão para pixel e sem correção de gamma.

Há uma única luz pontual e um único material para a cena; não há atenuação, sombras, múltiplas luzes nem texturas. A configuração desses parâmetros em arquivos separados é formalizada na [ADR-0010](0010-camera-iluminacao-arquivos-separados.md).

### Regras two-sided

1. Se `N·L < 0` **e** `V·N < 0`, inverter `N` e recalcular `N·L`.
2. Se ainda assim `N·L < 0`, desconsiderar as parcelas difusa e especular.
3. Se `R·V < 0`, desconsiderar apenas a parcela especular.

A parcela especular só é avaliada quando `N·L > 0`.

### Casos sem direção de iluminação

Retornam somente a parcela ambiente (`Ka · Iamb`, já limitada a `[0,255]`):

- normal nula;
- `P == Pl` (direção `L` nula);
- `P == C` (direção de vista nula);
- qualquer componente não finito de `P`, `N`, `C` ou `Pl`.

Componentes não finitos de `Iamb`, `Il`, `Ka`, `Kd`, `Od`, `Ks` ou `eta` não podem produzir `NaN`: a parcela afetada é zerada ou limitada, e o resultado permanece finito. A função nunca retorna canal `NaN` ou infinito.

### Escopo de tonalização

A entrega implementa **somente Phong shading**: normais e posição são interpoladas por pixel e a iluminação é calculada em cada pixel visível. Flat shading e Gouraud shading ficam fora do escopo definido para esta VA.

## Alternativas

- **Flat shading:** usa uma única normal por face e não interpola normais; produz facetas visíveis e não é o modelo pedido para a 2ª VA.
- **Gouraud shading:** calcula a cor por vértice e interpola cores; é mais barato, mas o destaque especular perde qualidade e o modelo de tonalização seria diferente do pedido.
- **Blinn-Phong:** usa `H = normalize(L + V)` no lugar de `R`; é uma aproximação diferente da equação de Phong do enunciado.
- **Múltiplas luzes, atenuação e sombras:** aumentariam o realismo, mas estão fora do escopo da 2ª VA e exigiriam parâmetros e algoritmos que o enunciado não pede.

## Consequências

- O custo da iluminação é por pixel visível, não por vértice ou por face.
- A iluminação é suave em superfícies com normais de vértice compartilhadas; arestas duras dependem de vértices duplicados.
- Não há correção de gamma: os canais são combinados e limitados diretamente em `[0,255]`.
- A cena permanece com uma única luz e um único material; múltiplas luzes, atenuação e sombras exigiriam uma nova decisão.
- A normal de face e a normal de vértice, incluindo a inversão two-sided, são formalizadas na [ADR-0008](0008-normais-face-vertice-two-sided.md).
- A configuração de câmera e iluminação em arquivos separados é formalizada na [ADR-0010](0010-camera-iluminacao-arquivos-separados.md).
- Os testes de iluminação usam valores calculados à mão e comparam canais RGB.
