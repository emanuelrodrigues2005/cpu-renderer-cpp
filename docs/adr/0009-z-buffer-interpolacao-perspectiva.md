# Z-buffer e interpolação perspectiva-correta no scan line

## Contexto

A 1ª VA rasterizava em branco, sem profundidade; a [ADR-0005](0005-convencoes-scan-line.md) fixou a varredura inclusiva, o arredondamento e o tratamento de arestas horizontais, mas não definia visibilidade. A 2ª VA exige z-buffer e tonalização de Phong por pixel, o que acrescenta dois problemas: decidir qual fragmento aparece e interpolar posição de mundo e normal sem a distorção da interpolação linear em tela.

## Decisão

### Profundidade

- Cada vértice projetado carrega `invZ = 1/z`, onde `z > 0` é a profundidade de vista.
- O buffer é limpo com `0`; o teste é `invZ > valor_atual`. Quanto maior o `invZ`, mais próximo o fragmento.
- Empate mantém o primeiro fragmento desenhado, o que torna a visibilidade determinística sem ordenar triângulos.
- Um `invZ` não finito nunca vence, mesmo que a comparação numérica o favoreça.
- Consultas e escritas fora dos limites são neutras: `at` devolve `0` e `testAndSet` devolve `false`, sem tocar a memória. Dimensões negativas no construtor viram `0`.

### Cobertura e interpolação

- A cobertura continua por **scan line**: ordenar os vértices por `y`, usar as interseções com as arestas não horizontais e preencher o intervalo `[x_min, x_max]` de cada linha; as convenções de arredondamento, inclusão das pontas e arestas horizontais da [ADR-0005](0005-convencoes-scan-line.md) permanecem.
- Para cada pixel candidato, coordenadas baricêntricas em tela fornecem os pesos. Pesos ligeiramente negativos por arredondamento são limitados a zero e os pesos são renormalizados.
- A profundidade é interpolada linearmente como `invZ`. Os atributos (posição de mundo e normal) são interpolados com pesos multiplicados por `invZ` e divididos pelo `invZ` final. `z` **nunca** é interpolado linearmente em tela.
- A normal interpolada é renormalizada por pixel. Se for nula ou não finita, usa-se a normal da face calculada com as posições de mundo dos três vértices; se essa também for nula ou não finita, usa-se o vetor nulo e a iluminação cai no caso de normal nula (somente ambiente).

### Limites e modos de depuração

- Não há ordenação de triângulos por baricentro: com z-buffer ela é desnecessária e pode ser incorreta para triângulos que se interceptam.
- Não há backface culling nem clipping contra o plano próximo.
- A projeção descarta vértices com `z <= 0` e rejeita coordenadas normalizadas com `|x_norm| > 1e5` ou `|y_norm| > 1e5` para evitar overflow na conversão para `int`. Como um triângulo parcialmente atrás da câmera não pode ser recortado nesta entrega, o rasterizador descarta o triângulo inteiro se qualquer vértice chegar com `invZ <= 0` ou não finito.
- Os modos `Points` e `Wireframe` permanecem sem profundidade e sem iluminação, como depuração.

## Alternativas

- **Painter's algorithm / ordenação por profundidade:** depende da ordem e falha em triângulos que se interceptam; o z-buffer resolve a visibilidade por pixel, sem ordenar.
- **`z` linear em tela:** é simples, mas a profundidade não é afim em coordenadas de tela; a interpolação de atributos ficaria distorcida. `invZ` é a grandeza afim.
- **Profundidade por triângulo:** usa um único valor por face; é grosseira para triângulos grandes e não resolve superfícies que se cruzam dentro do triângulo.
- **Clipping de plano próximo:** produziria recorte fiel de triângulos parcialmente atrás da câmera, mas está fora do escopo da 2ª VA; descartar o triângulo inteiro mantém o pipeline simples e previsível.

## Consequências

- A ordem de desenho dos triângulos não altera a visibilidade; empates são resolvidos pelo primeiro fragmento.
- A interpolação de atributos é perspectiva-correta; interpolar `z` linearmente seria errado.
- A ausência de clipping de plano próximo continua sendo uma limitação visível em zoom extremo.
- O buffer usa `std::vector<float>` do tamanho do canvas e é limpo a cada renderização; para 800×600 são cerca de 1,9 MB.
- A [ADR-0005](0005-convencoes-scan-line.md) é superada por esta decisão apenas no que se refere à visibilidade (z-buffer); as convenções de varredura, arredondamento e arestas horizontais continuam válidas.
- O comportamento é coberto por `depth_buffer` e `shaded_rasterizer` (ver [tests/test_depth_buffer.cpp](../../tests/test_depth_buffer.cpp) e [tests/test_shaded_rasterizer.cpp](../../tests/test_shaded_rasterizer.cpp)).
- A equação de iluminação que consome a normal interpolada está na [ADR-0007](0007-modelo-iluminacao-phong.md); a implementação está em [src/pipeline/DepthBuffer.cpp](../../src/pipeline/DepthBuffer.cpp) e [src/pipeline/ShadedRasterizer.cpp](../../src/pipeline/ShadedRasterizer.cpp).
