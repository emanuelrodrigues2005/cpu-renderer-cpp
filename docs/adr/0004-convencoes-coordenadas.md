# Convenções de coordenadas: N aponta para a cena e U = V × N

A base da câmera segue a convenção do enunciado: `N` aponta de `C` para a cena, `V` é ortogonalizado contra `N` (Gram-Schmidt) e `U = normalize(V × N)` fecha o triedro — com o `V` do exemplo, `U = (1,0,0)`, a direita do observador, evitando a imagem espelhada que `N × V` produziria. O triedro `(U, V, N)` resulta canhoto quando `V` aponta para baixo, e isso é deliberado: é a convenção que alimenta a projeção do PDF sem trocas de sinal. A câmera prepara e normaliza a base uma única vez na carga; arquivos com `N`/`V` nulos ou `V` paralelo a `N` são rejeitados com erro, e a carga segue o padrão atômico da ADR-0002 (cópia local; falha não toca a câmera de destino).

## Consequências

- `worldToView(p) = ((p−C)·U, (p−C)·V, (p−C)·N)`; pontos à frente têm `zv > 0`.
- A projeção, a normalização e a tela (#5) assumem este triedro; trocar o sinal de `U`/`V` espelha o resultado.
