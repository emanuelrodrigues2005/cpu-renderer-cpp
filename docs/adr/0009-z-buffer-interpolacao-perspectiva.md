# Z-buffer com interpolação perspectiva-correta no scan line

A visibilidade é resolvida por z-buffer. Para cada vértice projetado, a profundidade de vista `z > 0` é convertida em `invZ = 1/z`. O buffer é limpo com `0` e o teste é `invZ > valor_atual`: quanto maior o `invZ`, mais próximo o fragmento. Empates mantêm o primeiro fragmento desenhado, o que mantém a visibilidade determinística sem ordenar triângulos. Consultas e escritas fora dos limites são neutras: `at` devolve `0` e `testAndSet` devolve `false`, sem tocar a memória. Dimensões negativas no construtor viram `0`. Um `invZ` não finito nunca vence, mesmo que a comparação numérica o favoreça.

A cobertura continua sendo feita por **scan line**: os vértices são ordenados por `y`, as interseções com as arestas não horizontais definem o intervalo `[x_min, x_max]` de cada linha, e os pixels do intervalo são candidatos. Para cada pixel, coordenadas baricêntricas em tela fornecem os pesos; a profundidade é interpolada linearmente como `invZ`, e os atributos (posição de mundo e normal) são interpolados com pesos multiplicados por `invZ` e divididos pelo `invZ` final. A normal é renormalizada por pixel. Pesos ligeiramente negativos por arredondamento são limitados a zero e renormalizados. Se a normal interpolada for nula ou não finita, usa-se a normal da face, calculada a partir das posições de mundo dos três vértices; se essa também for nula ou não finita, usa-se o vetor nulo e a iluminação cai no caso de normal nula.

Não há ordenação de triângulos por baricentro: com z-buffer ela é desnecessária e pode ser incorreta para triângulos que se interceptam. Não há backface culling nem clipping contra o plano próximo. A projeção descarta vértices com profundidade de vista `z <= 0`; como um triângulo parcialmente atrás da câmera não pode ser recortado nesta entrega, o rasterizador descarta o triângulo inteiro se qualquer vértice chegar com `invZ <= 0` ou não finito. Como proteção contra overflow na conversão para `int`, coordenadas normalizadas acima de `1e5` também são rejeitadas na projeção. Os modos `Points` e `Wireframe` permanecem sem profundidade, como depuração.

## Consequências

- A ordem de desenho dos triângulos não altera a visibilidade.
- A interpolação de atributos é correta em perspectiva; interpolar `z` linearmente seria errado.
- A ausência de clipping de plano próximo continua sendo uma limitação visível em zoom extremo.
- O buffer usa `std::vector<float>` do tamanho do canvas e é limpo a cada renderização; para 800×600 são cerca de 1,9 MB.
- A ADR-0005 é superada por esta decisão apenas no que se refere à ausência de z-buffer; as convenções de varredura, arredondamento e horizontais continuam valendo.
