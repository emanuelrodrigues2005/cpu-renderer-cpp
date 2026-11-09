# Normais de face e de vértice e iluminação two-sided

## Contexto

O formato BYU usado pelo projeto descreve apenas vértices e triângulos; não há normais no arquivo. A tonalização de Phong por pixel precisa de uma normal em cada fragmento, e usar apenas a normal da face produziria facetas visíveis. Malhas abertas também aparecem com faces de winding invertido em relação à câmera/luz, o que exigiria iluminação nos dois lados para não escurecer partes do objeto.

## Decisão

- **Normal de face:** `normalize((b − a) × (c − a))`, com `a`, `b` e `c` nas posições de mundo dos vértices do triângulo. Faces com índices inválidos, área nula ou normal não finita não contribuem e ficam com vetor nulo.
- **Normal de vértice:** soma das normais unitárias das faces incidentes, seguida de normalização. É uma média **não ponderada**: a área e o ângulo de cada face não entram no cálculo. Vértice sem face válida incidente fica com vetor nulo. Faces cuja normal não é finita são rejeitadas e substituídas por vetor nulo; o resultado final é sempre finito.
- **Fallback na interpolação:** a normal interpolada é renormalizada. Se for nula ou não finita, usa-se a normal da face calculada com as posições de mundo dos três vértices. Se essa também for nula ou não finita, usa-se o vetor nulo e a iluminação cai no caso de normal nula, que retorna somente a parcela ambiente.
- **Two-sided:** com `N` unitária, `L` do ponto para a luz e `V` do ponto para a câmera:
  1. se `N·L < 0` **e** `V·N < 0`, inverter `N` e recalcular `N·L`;
  2. se ainda `N·L < 0`, desconsiderar as parcelas difusa e especular;
  3. se `R·V < 0`, desconsiderar apenas a parcela especular.

O cálculo da normal de face e de vértice é feito uma vez por renderização, em `renderFilledPhong` (via `computeMeshNormals`); a renormalização e o fallback acontecem por fragmento no rasterizador iluminado. A equação de iluminação e os demais casos especiais estão na [ADR-0007](0007-modelo-iluminacao-phong.md).

## Alternativas

- **Normais do BYU:** o formato usado não as traz e as variações de arquivo não são confiáveis; calcular no pipeline mantém a malha como fonte única de geometria.
- **Ponderação por área ou ângulo:** suaviza de forma diferente em malhas com triângulos de tamanhos variados, mas é mais complexa e não é exigida; a média não ponderada é a definida pelo projeto.
- **Sem two-sided:** faces com winding invertido ficariam pretas em malhas abertas; a inversão de `N` resolve o caso sem backface culling.
- **Flat shading:** usaria apenas a normal da face e não a normal de vértice; está fora do escopo da 2ª VA (ADR-0007).

## Consequências

- Vértices compartilhados recebem iluminação suave; arestas duras exigem vértices duplicados na malha.
- Faces degeneradas, nulas ou com índices inválidos são ignoradas e não contaminam as normais dos vértices.
- Malhas abertas são iluminadas dos dois lados, sem a necessidade de backface culling.
- A normal interpolada é renormalizada por pixel; o fallback para a normal da face cobre interpolações degeneradas.
- O comportamento é coberto por `normals`, `phong` e `renderer_phong` (ver [tests/test_normals.cpp](../../tests/test_normals.cpp)).
- O vocabulário de normal de face e normal de vértice está em [CONTEXT.md](../../CONTEXT.md) e a implementação em [src/mesh/Normals.cpp](../../src/mesh/Normals.cpp); a inversão two-sided está em [src/shading/Phong.cpp](../../src/shading/Phong.cpp).
