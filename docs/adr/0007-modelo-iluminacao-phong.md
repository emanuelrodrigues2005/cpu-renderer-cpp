# Modelo de iluminação de Phong e escopo somente Phong shading

A 2ª VA usa o modelo local de Phong com uma única luz pontual em coordenadas de mundo. Para um ponto `P` com normal unitária `N`, câmera em `C` e luz em `Pl`:

```text
L = normalize(Pl - P)
V = normalize(C - P)
R = 2(N·L)N - L

Ia = Ka · Iamb
Id = max(N·L, 0) · Kd ∗ Od ∗ Il
Is = max(R·V, 0)^eta · Ks · Il
I  = Ia + Id + Is
```

`Iamb` e `Il` são cores RGB em `[0,255]`; `Ka` e `Ks` estão em `[0,1]`; `Kd` e `Od` estão em `[0,1]`; `eta >= 1`. A multiplicação `∗` é componente a componente. O especular usa `Ks` e `Il`, nunca `Od`. A cor final é limitada a `[0,255]` por canal, com arredondamento na conversão para pixel e sem correção de gamma.

## Regras two-sided

1. Se `N·L < 0` **e** `V·N < 0`, inverter `N` e recalcular `N·L`.
2. Se ainda assim `N·L < 0`, desconsiderar as parcelas difusa e especular.
3. Se `R·V < 0`, desconsiderar apenas a parcela especular.

A parcela especular só é avaliada quando `N·L > 0`.

## Casos sem direção de iluminação

Retornam somente a parcela ambiente (`Ka · Iamb`, já limitada a `[0,255]`):

- normal nula;
- `P == Pl` (direção `L` nula);
- `P == C` (direção de vista nula);
- qualquer componente não finito de `P`, `N`, `C` ou `Pl`.

Componentes não finitos de `Iamb`, `Il`, `Ka`, `Kd`, `Od`, `Ks` ou `eta` não podem produzir `NaN`: a parcela afetada é zerada ou limitada, e o resultado permanece finito. A função nunca retorna canal `NaN` ou infinito.

## Escopo de tonalização

A entrega implementa **somente Phong shading**: normais e posição são interpoladas por pixel e a iluminação é calculada em cada pixel visível. Flat shading e Gouraud shading aparecem nos slides, mas ficam fora do escopo definido para esta VA. O caminho branco sem iluminação permanece como sobrecarga interna de `renderMesh` para os testes da 1ª VA; os modos de depuração `D`/`W` usam os caminhos de pontos/arame, também sem iluminação.

## Consequências

- A cor depende da posição do pixel, da normal interpolada e da posição da câmera; não há cor por vértice persistida.
- Há uma única luz pontual e um único material por malha; não há atenuação, sombras, múltiplas luzes nem texturas.
- Adicionar Gouraud no futuro exige calcular cores por vértice e interpolar cores, não apenas normais.
- Os testes de iluminação usam valores calculados à mão e comparam canais RGB.
- A normal de face e a normal de vértice são formalizadas na ADR-0008, e os arquivos de câmera/iluminação na ADR-0010; ambas ficam para #29.
