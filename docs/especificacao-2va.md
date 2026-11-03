# 2ª VA — Especificação (transcrição)

> Transcrição do enunciado em PDF entregue pelo professor (UFRPE/DC — Computação Gráfica Básica, 06230). O PDF original não é versionado neste repositório (material da disciplina).

## Objetivo

Alterar o programa da 1ª VA para renderizar um objeto 3D utilizando os modelos de tonalização e iluminação de Phong.

## Parâmetros carregados de arquivo texto

A aplicação deve carregar a partir de arquivo texto os seguintes parâmetros da câmera virtual e de iluminação:

- Ponto `C`;
- Vetores `N` e `V`;
- Escalares `d`, `hx` e `hy`;
- Cores RGB `Iamb` e `Il`;
- Escalares `Ka`, `Ks` e `η`;
- Vetores `Kd` e `Od`;
- Ponto `Pl`.

Exemplo de parâmetros de iluminação:

```text
Iamb = 100 100 100
Ka = 0.2
Il = 127 213 254
Pl = 60 5 -10
Kd = 0.5 0.3 0.2
Od = 0.7 0.5 0.8
Ks = 0.5
eta = 1
```

## Recarga em tempo de execução

O usuário deve ser capaz de alterar os valores dos parâmetros no arquivo texto, recarregá-los e redesenhar o objeto sem precisar fechar e abrir a aplicação novamente. Por exemplo, o usuário pode pressionar uma tecla específica para recarregar os parâmetros a partir do arquivo texto e redesenhar o objeto.

## Visibilidade e rasterização

- Utilizar o algoritmo de visibilidade **z-buffer**.
- Utilizar o algoritmo de rasterização de polígonos **scan line conversion** (varredura).

## Restrições

- Qualquer linguagem de programação pode ser utilizada (C, Java etc.).
- A única função gráfica permitida é a que desenha um pixel colorido na tela.
- Apenas as bibliotecas padrão da linguagem escolhida podem ser usadas.
- Caso desejado, é permitido usar uma biblioteca externa que ofereça a função de pintar um pixel colorido na tela.

> Nota: "apenas bibliotecas padrão" e "biblioteca externa permitida" não se contradizem — a exceção é exclusivamente a função de desenhar um pixel colorido. Neste projeto, essa exceção é a SDL2, usada somente para janela e `SDL_RenderDrawPoint` (ADR-0001).

## Decisões de implementação (não fazem parte do enunciado)

- O projeto usa C++17 e SDL2 apenas para janela e desenho de pixel.
- Câmera e iluminação são carregadas de dois arquivos separados; `R` recarrega ambos.
- O modelo de tonalização implementado é Phong shading; flat e Gouraud ficam fora do escopo desta entrega.
- A luz é pontual, em coordenadas de mundo, sem atenuação.
