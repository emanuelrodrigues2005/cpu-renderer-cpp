# CPU Renderer C++

Pipeline gráfico implementado manualmente em CPU: carrega malhas de triângulos, calcula normais, aplica uma câmera virtual, projeta em perspectiva, resolve visibilidade com z-buffer e rasteriza triângulos com *scan line*, calculando iluminação de Phong por pixel. A 1ª VA entregou a base (malha, câmera, projeção e rasterização); a 2ª VA acrescenta normais, z-buffer e iluminação/tonalização de Phong.

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
Sistema em que os vértices da malha, as normais e a fonte de luz estão definidos.
_Avoid_: espaço global

**Coordenadas de vista**:
Coordenadas de um ponto relativas à câmera, obtidas ao transladar por `C` e projetar sobre `U`, `V` e `N`.

**Projeção perspectiva**:
Mapeamento das coordenadas de vista para um plano 2D usando a distância focal; pontos atrás da câmera não são visíveis.

**Coordenadas normalizadas**:
Coordenadas 2D do plano de projeção escaladas pelos limites `hx` e `hy`, no intervalo `[-1, 1]` quando visíveis.

**Coordenadas de tela**:
Coordenadas de pixel resultantes da conversão das normalizadas para a janela, com `y` crescendo para baixo.

**Z-buffer**:
Buffer de profundidade por pixel que mantém a superfície mais próxima; o teste de profundidade decide qual fragmento é desenhado.

**invZ**:
Inverso da profundidade de vista (`1/z`), interpolado linearmente em coordenadas de tela; maior `invZ` indica fragmento mais próximo.

**Interpolação perspectiva-correta**:
Interpolação de atributos em coordenadas de tela ponderada por `1/z`, evitando distorções da interpolação linear.

**Normal de face**:
Vetor unitário perpendicular a um triângulo, calculado pelo produto vetorial de duas arestas.

**Normal de vértice**:
Normal suave de um vértice, obtida pela média não ponderada (soma das normais unitárias das faces incidentes, depois normalizada); vértice sem faces incidentes tem normal nula.

**Iluminação de Phong**:
Modelo local que combina reflexão ambiente, difusa e especular para determinar a cor de um ponto.

**Componente ambiente**:
Parcela `Ia = Ka · Iamb`, presente mesmo sem incidência direta de luz.

**Componente difusa**:
Parcela `Id = max(N·L, 0) · Kd ∗ Od ∗ Il`, proporcional ao cosseno do ângulo de incidência.

**Componente especular**:
Parcela `Is = max(R·V, 0)^eta · Ks · Il`, dependente da direção de vista e do expoente `eta`.

**Fonte de luz**:
Luz pontual em coordenadas de mundo, definida pela posição `Pl` e pela cor `Il`.

**Material**:
Conjunto de coeficientes e cores que caracterizam a superfície: `Ka`, `Kd`, `Od`, `Ks` e `eta`.

**Arquivo de iluminação**:
Arquivo texto com os oito parâmetros da iluminação: `Iamb`, `Ka`, `Il`, `Pl`, `Kd`, `Od`, `Ks` e `eta`. O padrão do projeto é `lighting/lighting.txt`; `lighting/lighting-pdf.txt` preserva o exemplo do PDF e não é o padrão.

**Vetor L**:
Direção unitária do ponto iluminado para a fonte de luz.
_Avoid_: vetor de luz da câmera

**Vetor V (iluminação)**:
Direção unitária do ponto iluminado para a câmera; não é o vetor `V` da base da câmera.
_Avoid_: V da câmera

**Vetor R**:
Direção unitária de reflexão da luz incidente em relação à normal.

**Modelo de tonalização**:
Define quando o modelo de iluminação é aplicado e quais valores recebe: por face, por vértice ou por pixel.

**Tonalização de Phong**:
Modelo de tonalização que interpola normais e posição por pixel e calcula a iluminação em cada pixel visível.
_Avoid_: Gouraud, flat shading

**Renderização iluminada**:
Renderização que recebe a iluminação da cena, calcula normais e profundidade e tonaliza cada pixel com Phong.

**Caminho legado**:
Renderização sem iluminação e sem profundidade, usada para preencher em branco e para os modos de depuração.

**Scan line**:
Preenchimento de um triângulo projetado por varredura: para cada linha horizontal, calcula as interseções com as arestas e pinta o intervalo entre elas.
_Avoid_: flood fill

**Recarga da configuração** (também chamada de recarga da cena):
Releitura dos arquivos correntes de câmera e iluminação pela tecla `R`, em tempo de execução, sem reiniciar a aplicação. Cada fonte é carregada atomicamente; erro em uma fonte preserva o último valor válido dessa fonte, reporta no `stderr` e não invalida a outra.
_Avoid_: recarga da câmera

**Câmera compartilhada**:
Único arquivo `camera/camera.txt` usado por todas as malhas; trocar de malha não altera a câmera nem a iluminação, e `R` recarrega câmera e iluminação para a visualização corrente.
_Avoid_: preset de câmera, câmera por modelo

**Canvas**:
Destino de desenho que expõe `clear`, `drawPixel`, `width()` e `height()`.

**Dump BMP**:
Imagem gerada sem janela, usada para inspeção e testes automatizados.
