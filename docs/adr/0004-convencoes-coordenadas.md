# Convenções de coordenadas: N aponta para a cena e U = V × N

A base da câmera segue a convenção do enunciado: `N` aponta de `C` para a cena, `V` é ortogonalizado contra `N` (Gram-Schmidt) e `U = normalize(V × N)` fecha o triedro — com o `V` do exemplo, `U = (1,0,0)`, a direita do observador, evitando a imagem espelhada que `N × V` produziria. O triedro `(U, V, N)` resulta canhoto quando `V` aponta para baixo, e isso é deliberado: é a convenção que alimenta a projeção do PDF sem trocas de sinal. A câmera prepara e normaliza a base uma única vez na carga; arquivos com `N`/`V` nulos ou `V` paralelo a `N` são rejeitados com erro, e a carga segue o padrão atômico da ADR-0002 (cópia local; falha não toca a câmera de destino).

## Projeção, normalização e tela

A projeção perspectiva é `xp = d·xv/zv` e `yp = d·yv/zv`; a normalização divide por `hx`/`hy`, colocando o que está visível em `[-1, 1]`; a tela mapeia `screenX = (x_norm+1)·largura/2` e `screenY = (1−y_norm)·altura/2`, com `y` crescendo para baixo, e arredonda com `std::lround`. Pontos com `zv ≤ 0` estão atrás da câmera e não são visíveis. Não há clipping: triângulos com qualquer vértice não visível são descartados inteiros — recortar as partes visíveis seria mais fiel, mas exige um algoritmo fora do escopo da 1ª VA e descartar mantém o pipeline simples e previsível.

## Consequências

- `worldToView(p) = ((p−C)·U, (p−C)·V, (p−C)·N)`; pontos à frente têm `zv > 0`.
- A projeção, a normalização e a tela assumem este triedro; trocar o sinal de `U`/`V` espelha o resultado.
- Pontos com `zv ≤ 0` são invisíveis; triângulos que os usam são descartados inteiros, sem recorte parcial.
