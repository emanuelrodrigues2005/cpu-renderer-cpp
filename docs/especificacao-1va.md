# 1ª VA — Especificação (transcrição)

> Transcrição do enunciado em PDF entregue pelo professor (UFRPE/DC — Computação Gráfica Básica, 06230). O PDF original não é versionado neste repositório (material da disciplina).

## Objetivo

Carregar na memória uma malha de triângulos referente a um objeto 3D armazenada em arquivo de texto e desenhar o objeto na tela.

> Observação: o enunciado abre falando em "desenhar seus vértices na tela", mas o corpo da especificação exige projeção em perspectiva, conversão para coordenadas de tela e preenchimento dos triângulos com *scan line conversion*. A implementação segue o corpo completo do enunciado.

## Formato do arquivo de malha

```
<nº de vértices> <nº de triângulos>
<coordenada x do vértice 1> <coordenada y do vértice 1> <coordenada z do vértice 1>
...
<coordenada x do vértice n> <coordenada y do vértice n> <coordenada z do vértice n>
<índice 1 do triângulo 1> <índice 2 do triângulo 1> <índice 3 do triângulo 1>
...
<índice 1 do triângulo k> <índice 2 do triângulo k> <índice 3 do triângulo k>
```

Exemplo (primeiro objeto de `models/piramide.byu`):

```
4 4
1 1 1
1 30 1
30 30 1
1 1 30
1 2 3
1 2 4
2 3 4
1 3 4
```

## Parâmetros da câmera

A aplicação deve carregar de um arquivo de texto:

- Ponto `C`;
- Vetores `N` e `V`;
- Escalares `d`, `hx` e `hy`.

Exemplo (`camera/camera.txt`):

```
N = 0 1 -1
V = 0 -1 -1
d = 5
hx = 2
hy = 2
C = 0 -500 500
```

O usuário deve ser capaz de alterar os valores no arquivo, recarregá-los e redesenhar o objeto **sem fechar a aplicação** (por exemplo, pressionando uma tecla específica).

## Pipeline

1. Converter os vértices de coordenadas mundiais para coordenadas de vista;
2. Aplicar a projeção em perspectiva;
3. Converter para coordenadas normalizadas;
4. Converter para coordenadas de tela;
5. Rasterizar os triângulos projetados com *scan line conversion* (varredura).

## Cores

Os pixels correspondentes aos triângulos preenchidos devem ser **brancos**; os demais pixels, **pretos**.

## Restrições

- Qualquer linguagem pode ser utilizada; este projeto usa C++.
- A única função gráfica permitida é a que desenha um pixel colorido na tela.
- Apenas as bibliotecas padrão da linguagem; é permitida uma biblioteca externa que ofereça a função de pintar um pixel colorido na tela (SDL2, neste projeto).
