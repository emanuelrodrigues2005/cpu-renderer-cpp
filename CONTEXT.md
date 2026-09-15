# CPU Renderer C++

Pipeline gráfico implementado manualmente em CPU: carrega malhas de triângulos, aplica uma câmera virtual, projeta em perspectiva e rasteriza triângulos com *scan line*.

## Linguagem

**Malha (Mesh)**:
Conjunto de vértices e triângulos que descreve um objeto 3D.
_Avoid_: modelo, objeto, geometria

**Vértice (Vertex)**:
Ponto 3D em coordenadas de mundo que compõe uma malha.

**Triângulo (Triangle)**:
Face primitiva da malha, definida por três índices de vértices; 1-based no arquivo, 0-based em memória.

**Arquivo BYU**:
Formato texto com a contagem de vértices e triângulos, seguida das coordenadas dos vértices e dos índices dos triângulos; conteúdo extra após o último triângulo é ignorado.
_Avoid_: arquivo de modelo

**Câmera**:
Base ortonormal `(U, V, N)`, centro `C` e escalares `d`, `hx`, `hy` que definem a projeção.

**Centro da câmera (C)**:
Ponto de origem do sistema de coordenadas de vista.

**Base da câmera (U, V, N)**:
`N` aponta da câmera para a cena; `V` é o vetor vertical ortogonalizado contra `N`; `U` completa a base.
_Avoid_: eixos da câmera

**Coordenadas de mundo**:
Sistema em que os vértices da malha estão definidos.
_Avoid_: espaço global

**Coordenadas de vista**:
Coordenadas de um ponto relativas à câmera, obtidas ao transladar por `C` e projetar sobre `U`, `V` e `N`.

**Projeção perspectiva**:
Mapeamento das coordenadas de vista para um plano 2D usando a distância focal; pontos atrás da câmera não são visíveis.

**Coordenadas normalizadas**:
Coordenadas 2D do plano de projeção escaladas pelos limites `hx` e `hy`, no intervalo `[-1, 1]` quando visíveis.

**Coordenadas de tela**:
Coordenadas de pixel resultantes da conversão das normalizadas para a janela, com `y` crescendo para baixo.

**Scan line**:
Preenchimento de um triângulo projetado por varredura: para cada linha horizontal, calcula as interseções com as arestas e pinta o intervalo entre elas.
_Avoid_: flood fill, z-buffer

**Recarga da câmera**:
Releitura de `camera.txt` em tempo de execução, sem reiniciar a aplicação; em caso de erro, a última câmera válida é mantida.

**Câmera compartilhada**:
Único arquivo `camera/camera.txt` usado por todas as malhas; trocar de malha não altera a câmera, e `R` a relê para a visualização corrente.
_Avoid_: preset de câmera, câmera por modelo

**Canvas**:
Destino de desenho que expõe apenas `clear` e `drawPixel`.

**Dump BMP**:
Imagem gerada sem janela, usada para inspeção e testes automatizados.
