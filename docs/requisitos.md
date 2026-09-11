# Documento de Levantamento de Requisitos - Projeto de Computação Gráfica

## 1. Identificação do Projeto

**Nome:** Implementação de um Pipeline Gráfico com Rasterização de
Triângulos

**Linguagem definida:** C++

**Bibliotecas permitidas/recomendadas:** - C++ Standard Library - SDL2
(somente para criação da janela e desenho de pixels)

**Objetivo geral:**

Implementar um pipeline gráfico simplificado, reproduzindo as etapas
fundamentais de uma GPU:

1.  Leitura de modelos 3D;
2.  Processamento de câmera;
3.  Transformação de coordenadas;
4.  Projeção perspectiva;
5.  Conversão para coordenadas de tela;
6.  Rasterização de triângulos utilizando algoritmo Scan Line.

O projeto não deve utilizar motores gráficos ou APIs de alto nível como
OpenGL, Unity ou Three.js. A lógica principal de transformação e
rasterização deve ser desenvolvida manualmente.

------------------------------------------------------------------------

# 2. Visão Geral do Sistema

O sistema desenvolvido será responsável por receber um modelo
tridimensional formado por vértices e triângulos, aplicar uma câmera
virtual e gerar uma imagem 2D através do preenchimento dos pixels
correspondentes aos triângulos projetados.

Fluxo principal:

    Arquivo BYU
        |
        v
    Leitura dos vértices
        |
        v
    Leitura dos triângulos
        |
        v
    Configuração da câmera
        |
        v
    Transformação Mundo -> Vista
        |
        v
    Projeção Perspectiva
        |
        v
    Normalização
        |
        v
    Conversão para coordenadas de tela
        |
        v
    Rasterização Scan Line
        |
        v
    Imagem final

------------------------------------------------------------------------

# 3. Requisitos Funcionais

## RF01 - Carregamento de modelos 3D

O sistema deve permitir a leitura de arquivos no formato BYU contendo:

-   Quantidade de vértices;
-   Coordenadas tridimensionais dos vértices;
-   Definição dos triângulos através dos índices dos vértices.

O modelo carregado deve ser armazenado internamente.

Estruturas esperadas:

``` cpp
struct Vertex {
    float x;
    float y;
    float z;
};

struct Triangle {
    int a;
    int b;
    int c;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};
```

------------------------------------------------------------------------

## RF02 - Implementação de operações vetoriais

O sistema deve possuir uma estrutura matemática para manipulação de
vetores 3D.

Operações necessárias:

-   Soma de vetores;
-   Subtração;
-   Produto escalar;
-   Produto vetorial;
-   Normalização;
-   Cálculo de magnitude.

------------------------------------------------------------------------

## RF03 - Configuração da câmera

O sistema deve carregar os parâmetros da câmera:

-   Vetor V;
-   Vetor N;
-   Vetor U;
-   Centro da câmera;
-   Distância focal;
-   Limites horizontais e verticais.

A câmera deve ser preparada através das etapas:

1.  Ortogonalização de V;
2.  Normalização de V e N;
3.  Cálculo de U.

------------------------------------------------------------------------

## RF04 - Transformação Mundo para Vista

Cada vértice do modelo deve ser transformado do sistema de coordenadas
mundial para o sistema de coordenadas da câmera.

Processo:

1.  Transladar o ponto em relação à posição da câmera;
2.  Projetar sobre os eixos U, V e N.

Resultado:

Cada vértice deve possuir coordenadas relativas à câmera.

------------------------------------------------------------------------

## RF05 - Projeção Perspectiva

O sistema deve converter coordenadas tridimensionais em coordenadas
bidimensionais.

Equações esperadas:

    xp = d * xv / zv

    yp = d * yv / zv

Onde:

-   d representa a distância da câmera;
-   xv e yv são coordenadas no sistema de vista.

------------------------------------------------------------------------

## RF06 - Conversão para coordenadas de tela

As coordenadas projetadas devem ser convertidas para pixels.

Exemplo para uma tela 800x600:

    screenX = (x_normalizado + 1) * largura / 2

    screenY = (1 - y_normalizado) * altura / 2

------------------------------------------------------------------------

## RF07 - Rasterização de triângulos

O sistema deve preencher cada triângulo projetado utilizando o algoritmo
Scan Line.

Para cada triângulo:

1.  Identificar limites superior e inferior;
2.  Percorrer cada linha horizontal;
3.  Calcular interseções da linha com as arestas;
4.  Preencher os pixels entre as interseções.

------------------------------------------------------------------------

## RF08 - Renderização dos pixels

O sistema deve possuir uma função simples responsável por desenhar
pixels.

Exemplo:

``` cpp
drawPixel(x, y, color);
```

A SDL2 será utilizada somente para esta camada.

------------------------------------------------------------------------

# 4. Requisitos Não Funcionais

## RNF01 - Linguagem

O projeto deve ser desenvolvido em C++.

Motivos:

-   Melhor controle matemático;
-   Adequação à área de Computação Gráfica;
-   Facilidade de trabalhar com estruturas geométricas;
-   Maior desempenho.

------------------------------------------------------------------------

## RNF02 - Organização

O código deve possuir separação de responsabilidades.

Cada módulo deve possuir uma função clara.

------------------------------------------------------------------------

## RNF03 - Manutenibilidade

O projeto deve possuir:

-   Código organizado;
-   Classes independentes;
-   Nomes claros;
-   Comentários onde necessário.

------------------------------------------------------------------------

# 5. Arquitetura Recomendada

Estrutura de diretórios:

    ProjetoCG/

    ├── src/
    │
    │   ├── main.cpp
    │
    │   ├── Vec3.h
    │   ├── Vec3.cpp
    │
    │   ├── Mesh.h
    │   ├── Mesh.cpp
    │
    │   ├── Camera.h
    │   ├── Camera.cpp
    │
    │   ├── Projection.h
    │   ├── Projection.cpp
    │
    │   ├── Rasterizer.h
    │   ├── Rasterizer.cpp
    │
    │   ├── Renderer.h
    │   ├── Renderer.cpp
    │
    │   └── WindowSDL.h
    │
    ├── models/
    │
    │   ├── vaso.byu
    │   ├── piramide.byu
    │   ├── triangulo.byu
    │   └── maca.byu
    │
    ├── camera/
    │
    │   └── camera.txt
    │
    ├── CMakeLists.txt
    │
    └── README.md

------------------------------------------------------------------------

# 6. Descrição dos Módulos

## Vec3

Responsável pela matemática vetorial.

Responsabilidades:

-   Vetores 3D;
-   Operações matemáticas;
-   Normalização.

------------------------------------------------------------------------

## Mesh

Responsável pela leitura e armazenamento dos modelos.

Responsabilidades:

-   Abrir arquivo BYU;
-   Criar lista de vértices;
-   Criar lista de triângulos.

------------------------------------------------------------------------

## Camera

Responsável pela construção da câmera virtual.

Responsabilidades:

-   Carregar parâmetros;
-   Calcular base U,V,N;
-   Realizar transformação mundo-vista.

------------------------------------------------------------------------

## Projection

Responsável pela projeção.

Responsabilidades:

-   Perspectiva;
-   Normalização;
-   Conversão para tela.

------------------------------------------------------------------------

## Rasterizer

Responsável pelo preenchimento dos triângulos.

Responsabilidades:

-   Algoritmo Scan Line;
-   Cálculo de interseções;
-   Pintura dos pixels.

------------------------------------------------------------------------

## Renderer

Responsável por integrar todos os módulos.

Fluxo:

    Mesh
     |
    Camera
     |
    Projection
     |
    Rasterizer
     |
    Tela

------------------------------------------------------------------------

# 7. Plano de Desenvolvimento

## Fase 1 - Preparação

Objetivos:

-   Criar projeto C++;
-   Configurar SDL2;
-   Abrir janela.

Critério de conclusão:

Uma janela vazia funcionando.

------------------------------------------------------------------------

## Fase 2 - Leitura dos modelos

Objetivos:

-   Ler arquivos BYU;
-   Armazenar vértices e triângulos.

Critério:

Exibir quantidade de vértices e faces.

------------------------------------------------------------------------

## Fase 3 - Matemática

Objetivos:

-   Implementar Vec3;
-   Validar operações.

------------------------------------------------------------------------

## Fase 4 - Câmera

Objetivos:

-   Criar sistema U,V,N;
-   Implementar mundo-vista.

------------------------------------------------------------------------

## Fase 5 - Projeção

Objetivos:

-   Aplicar perspectiva;
-   Converter para tela.

------------------------------------------------------------------------

## Fase 6 - Rasterização

Objetivos:

-   Implementar Scan Line;
-   Renderizar triângulos preenchidos.

------------------------------------------------------------------------

# 8. Estratégia Inicial de Implementação

O desenvolvimento deve começar pelo menor caso possível.

Ordem recomendada:

1.  Abrir uma janela;
2.  Desenhar um pixel;
3.  Desenhar pontos;
4.  Desenhar linhas;
5.  Ler um triângulo simples;
6.  Projetar esse triângulo;
7.  Preencher com Scan Line;
8.  Renderizar modelos maiores.

------------------------------------------------------------------------

# 9. Critérios de Aceitação

O projeto será considerado funcional quando:

-   Arquivos BYU forem carregados corretamente;
-   A câmera puder ser configurada;
-   Os vértices forem projetados corretamente;
-   Triângulos forem convertidos para pixels;
-   A rasterização preencher os objetos na tela;
-   Diferentes modelos puderem ser renderizados.

------------------------------------------------------------------------

# 10. Conclusão

Este projeto representa a construção manual de um pipeline gráfico
simplificado.

A implementação deve ser feita incrementalmente, validando cada etapa
antes da próxima.

A tecnologia recomendada é:

**C++ + SDL2 + CMake**

pois fornece controle suficiente para implementar todas as etapas
exigidas sem adicionar complexidade desnecessária.
