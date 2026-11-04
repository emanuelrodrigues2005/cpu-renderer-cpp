# Requisitos — 2ª VA

Documento derivado de `docs/especificacao-2va.md`, dos slides de iluminação/tonalização e das convenções do projeto. A 1ª VA (`docs/requisitos-1va.md`) permanece como pré-requisito: malha BYU, câmera, projeção, scan line e canvas.

> **Proveniência.** Cada requisito indica sua origem na coluna "Origem" da matriz de rastreabilidade. `PDF/slides` é o enunciado da disciplina; `projeto` são convenções e decisões deste repositório (`AGENTS.md`, `CONTEXT.md`, ADRs e regressão da 1ª VA). Um requisito marcado como `projeto` não é exigência do PDF e não deve ser apresentado como tal.

As decisões de implementação estão separadas na seção 7 e nas ADRs; não são requisitos.

## 1. Requisitos funcionais

### RF09 — Carregar parâmetros de iluminação

Carregar de arquivo texto os oito parâmetros:

- `Iamb`: cor RGB da luz ambiental, canais em `[0,255]`;
- `Ka`: coeficiente de reflexão ambiental, em `[0,1]`;
- `Il`: cor RGB da fonte de luz, canais em `[0,255]`;
- `Pl`: posição da fonte de luz em coordenadas de mundo;
- `Kd`: coeficiente de reflexão difusa, vetor em `[0,1]`;
- `Od`: cor difusa do material, vetor em `[0,1]`;
- `Ks`: coeficiente de reflexão especular, em `[0,1]`;
- `eta`: expoente especular, `>= 1`.

A leitura deve ser estrita (regras na seção 3.4), atômica e reportar erros em português. Arquivo inválido não pode alterar a iluminação anteriormente válida.

*Origem: PDF/slides (parâmetros); projeto (faixas numéricas e validação estrita).*

### RF10 — Calcular normais

Calcular a normal de cada face pelo produto vetorial das arestas e a normal de cada vértice pela média não ponderada das normais unitárias das faces incidentes, seguida de normalização. Regras verificáveis:

- faces degeneradas ou de vetor nulo não contribuem;
- vértice sem faces válidas fica com normal nula;
- normal não finita é rejeitada e substituída por vetor nulo;
- o resultado deve ser sempre finito.

*Origem: projeto (derivado da equação de Phong do PDF, que exige `N`; o formato BYU não traz normais).*

### RF11 — Iluminação de Phong

Calcular a cor de um ponto `P` por:

```text
Ia = Ka * Iamb
Id = max(N·L, 0) * Kd * Od * Il
Is = max(R·V, 0)^eta * Ks * Il
I  = Ia + Id + Is
```

com `L = normalize(Pl - P)`, `V = normalize(C - P)`, `R = 2(N·L)N - L`, multiplicação componente a componente. O especular **não** é tingido por `Od`. O `V` da iluminação é a direção de vista, distinta do vetor `V` da base da câmera.

Aplicar as três regras de iluminação two-sided:

1. se `N·L < 0` **e** `V·N < 0`, inverter `N`;
2. se ainda `N·L < 0`, desconsiderar as parcelas difusa e especular;
3. se `R·V < 0`, desconsiderar apenas a parcela especular.

O resultado é limitado por canal a `[0,255]`, com arredondamento e sem correção gamma.

*Origem: PDF/slides (equação e two-sided); projeto/AGENTS (V de vista, clamp com arredondamento, sem gamma).*

### RF12 — Tonalização de Phong

Interpolar a posição de mundo e a normal por pixel, com correção de perspectiva, e calcular a iluminação de Phong (RF11) para cada pixel visível. A normal interpolada deve ser normalizada; se for nula, usar a normal da face.

*Origem: PDF (tonalização de Phong); projeto (correção de perspectiva, normalização e fallback).*

### RF13 — Z-buffer

Manter um buffer de profundidade por pixel no caminho iluminado. Armazenar `invZ = 1/z`; o maior `invZ` vence e o empate mantém o primeiro fragmento. A visibilidade não pode depender da ordem de desenho dos triângulos. Os modos de depuração `Points` e `Wireframe` permanecem sem profundidade e sem iluminação.

*Origem: PDF (z-buffer); projeto/AGENTS (invZ, empate e caminho iluminado); ADR-0009 (#24).*

### RF14 — Scan line

Rasterizar cada triângulo projetado por varredura: ordenar por `y`, calcular as interseções com as arestas não horizontais e preencher o intervalo entre elas. A interpolação de atributos deve usar `invZ` (RF13) para correção de perspectiva; **nunca** interpolar `z` linearmente em tela.

*Origem: PDF (scan line); projeto/ADR-0005 (cobertura); projeto/AGENTS/ADR-0009 (invZ).*

### RF15 — Recarga da configuração

O usuário deve poder alterar os valores no(s) arquivo(s) de configuração corrente(s) — câmera e iluminação —, recarregá-los e redesenhar sem reiniciar a aplicação. A recarga usa os caminhos correntes: os padrões definidos pela configuração do projeto ou os caminhos alternativos informados na linha de comando.

Cada fonte de configuração (câmera e iluminação) é carregada atomicamente; erro preserva o último valor válido daquela fonte e reporta no `stderr`. A falha na recarga de uma fonte não pode invalidar a outra. O objeto deve ser redesenhado se ao menos uma fonte recarregar com sucesso.

*Origem: PDF (recarga e redesenho sem reiniciar); projeto (caminhos correntes, tecla de recarga e atomicidade).*

### RF16 — Integração e saída

Manter os controles existentes (`D`, `W`, `+`/`-`, `1`–`6`, `F11`, `Esc`/`Q`), a janela redimensionável, o dump BMP headless, `--info`, `--smoke`, `--test-pattern` e `--fullscreen`. Os controles `1`–`6` trocam apenas a malha; câmera e iluminação são compartilhadas.

Os caminhos padrão de malha, câmera e iluminação são definidos pela configuração do projeto (origem: projeto) e podem ser substituídos pelos argumentos posicionais da linha de comando.

*Origem: projeto/regressão da 1ª VA.*

## 2. Requisitos não funcionais

### RNF04 — Biblioteca gráfica

C++17 e biblioteca padrão. SDL2 apenas para criação da janela e desenho de pixel (`SDL_RenderDrawPoint`). Proibido usar texturas, `SDL_RenderDrawLine`, `SDL_RenderFillRect`, primitivas de triângulo, `SDL_RenderCopy`, OpenGL, Vulkan ou motores gráficos.

*Origem: PDF (pixel/bibliotecas); projeto/1VA (C++17); projeto/ADR-0001 (SDL2).*

### RNF05 — Testes headless

O pipeline deve rodar sem servidor gráfico por `MemoryCanvas`, com testes de unidade e integração via CTest. Dumps BMP não são baselines versionadas.

*Origem: projeto/ADR-0003.*

### RNF06 — Desempenho mensurável

Um redesenho completo no pipeline headless — `renderMesh` com `MemoryCanvas`, sem `WindowSDL::clear`, sem `SDL_RenderDrawPoint` e sem `present` — em 800×600, de qualquer um dos 6 modelos, deve concluir em até 100 ms. A medição usa build Release sem sanitizers, mediana de 10 redesenhos após aquecimento, em máquina x86-64 com pelo menos 4 núcleos e 8 GB de RAM. O desenho da janela não entra nesse orçamento. A máquina exata e o procedimento de medição são registrados no relatório de #27.

*Origem: projeto (orçamento definido para tornar "interativo" verificável; a cadência de redesenho é decisão da ADR-0001, não requisito).*

## 3. Detalhes verificáveis

### 3.1 Iluminação de Phong

- `L = normalize(Pl − P)`, `V = normalize(C − P)`, `R = 2(N·L)N − L`.
- `Ia = Ka·Iamb`; `Id = max(N·L,0)·Kd∗Od∗Il`; `Is = max(R·V,0)^eta·Ks·Il`.
- `Iamb`/`Il` em `[0,255]`; `Ka`/`Ks` em `[0,1]`; `Kd`/`Od` em `[0,1]`; `eta >= 1`.
- O especular usa `Ks` e `Il`, nunca `Od`.
- Casos especiais: (a) `N·L < 0` e `V·N < 0` → inverter `N`; (b) se ainda `N·L < 0` → sem difusa e sem especular; (c) `R·V < 0` → sem especular.
- Clamp final por canal em `[0,255]`, com arredondamento; sem gamma.

*Origem: PDF/slides (equação e two-sided); projeto/AGENTS (V de vista, clamp com arredondamento, sem gamma).*

### 3.2 Interpolação e normais

- Atributos (posição de mundo e normal) são interpolados com pesos baricêntricos multiplicados por `invZ` e divididos pelo `invZ` final.
- Nunca interpolar `z` linearmente em tela.
- Normalizar a normal após interpolar; se ela for nula, usar a normal da face.
- Faces degeneradas ou de vetor nulo não contribuem; vértice sem faces válidas fica com normal nula; normal não finita é rejeitada e substituída por vetor nulo; o resultado é sempre finito.

*Origem: projeto/AGENTS; ADR-0009 (#24).*

### 3.3 Z-buffer e scan line

- Armazenar `invZ = 1/z`; maior `invZ` vence; empate mantém o primeiro fragmento.
- Cobertura por scan line: ordenar por `y`, usar interseções com arestas não horizontais e preencher o intervalo entre elas.
- O intervalo é pintado de `lround(x_min)` a `lround(x_max)`, inclusive; triângulos de área zero são descartados; a varredura é limitada ao `Canvas`.
- Z-buffer apenas no caminho iluminado; `Points` e `Wireframe` continuam sem profundidade e sem iluminação.
- Não ordenar triângulos por baricentro: a visibilidade é resolvida pelo z-buffer.

*Origem: PDF (z-buffer e scan line); projeto/ADR-0005 (cobertura); projeto/AGENTS/ADR-0009 (invZ e modos de depuração).*

### 3.4 Parser e recarga

- Todos os 8 campos de iluminação são obrigatórios: `Iamb`, `Ka`, `Il`, `Pl`, `Kd`, `Od`, `Ks`, `eta`.
- Sem chaves duplicadas e sem tokens extras; qualquer ausência ou excesso é erro.
- Todos os valores devem ser finitos.
- Faixas: `Iamb`/`Il` em `[0,255]`; `Ka`/`Ks` em `[0,1]`; `Kd`/`Od` em `[0,1]`; `eta >= 1`.
- Mensagens de erro em português, citando o campo e o caminho do arquivo.
- Carga atômica: parsear em cópia local e só substituir o estado após sucesso; erro preserva o último valor válido.
- O arquivo de câmera mantém as regras da 1ª VA (seis campos, base ortonormal e escalares positivos).
- `R` recarrega câmera e iluminação a partir dos caminhos correntes; a troca de malha não recarrega nem troca a configuração.

*Origem: PDF (recarga); projeto (validação estrita, mensagens e atomicidade); ADR-0002 e ADR-0004; ADR-0006 (câmera compartilhada).*

### 3.5 Integração e saída

- `--info` imprime contagens de vértices e triângulos da malha indicada.
- `--dump` grava BMP 24-bit sem abrir janela.
- `--smoke` executa o pipeline headless e retorna sucesso.
- `--test-pattern` desenha o padrão de teste na janela; `--fullscreen` inicia em tela cheia.
- `--dump` e `--smoke` usam o caminho iluminado.
- Os caminhos padrão são fornecidos pela configuração do projeto e substituíveis na linha de comando.

*Origem: projeto/regressão da 1ª VA.*

## 4. Critérios de aceitação da 2ª VA

- [ ] Carrega os 8 parâmetros de iluminação, com validação estrita (campos obrigatórios, sem duplicatas, sem tokens extras, valores finitos e dentro das faixas).
- [ ] Arquivo de iluminação inválido gera erro em português citando campo e caminho, e preserva a última iluminação válida (teste de #20).
- [ ] `R` recarrega câmera e iluminação pelos caminhos correntes sem reiniciar; cada fonte de configuração é carregada atomicamente, erro em uma preserva o último valor válido daquela fonte e reporta no `stderr`; o objeto é redesenhado se ao menos uma fonte recarregar com sucesso (teste de #26).
- [ ] Caminhos alternativos informados na linha de comando são usados na carga e na recarga.
- [ ] Calcula normais de face e de vértice para os 6 modelos; faces degeneradas ou de vetor nulo não contribuem; vértice sem faces válidas fica com normal nula; normal não finita é rejeitada e substituída por vetor nulo; todas as normais resultantes são finitas (teste de #21, incluindo caso de normal não finita).
- [ ] Aplica a equação de Phong com ambiente, difusa e especular; o especular não é tingido por `Od` (teste de #23).
- [ ] Aplica as regras two-sided: (a) inverte `N` quando `N·L < 0` e `V·N < 0`; (b) remove difusa e especular se ainda `N·L < 0`; (c) remove só o especular se `R·V < 0` (teste de #23).
- [ ] Aplica clamp por canal em `[0,255]` com arredondamento e sem gamma (teste de #23).
- [ ] Renderiza com tonalização de Phong por pixel: interpolação com `invZ`, normal normalizada e fallback para a normal da face (testes de #24 e #25).
- [ ] Usa z-buffer com `invZ`: o fragmento mais próximo vence independentemente da ordem de desenho; empate mantém o primeiro (testes de #22 e #24).
- [ ] Usa scan line para a cobertura dos triângulos, com interseções em arestas não horizontais e intervalo preenchido (teste de #24).
- [ ] `Points` e `Wireframe` continuam sem profundidade e sem iluminação.
- [ ] Os 6 modelos renderizam sem crash, com pelo menos 500 pixels não pretos e pelo menos 8 cores distintas por modelo, além de variação de cor entre pixels; um único pixel não é evidência suficiente (testes de #27).
- [ ] `--dump`, `--smoke`, `--info`, `--test-pattern` e `--fullscreen` continuam funcionando.
- [ ] Apenas SDL para janela/pixel; matemática, interpolação e rasterização manuais.
- [ ] Redesenho headless (`renderMesh` + `MemoryCanvas`, sem SDL) em 800×600 em até 100 ms, mediana de 10 após aquecimento, build Release sem sanitizers, em x86-64 com pelo menos 4 núcleos e 8 GB de RAM; o desenho da janela não entra no orçamento (teste de #27).
- [ ] Testes headless verdes e build sem warnings.

## 5. Rastreabilidade

| Requisito | Issue | Origem |
|---|---|---|
| RF09 | #20 | PDF/slides (parâmetros); projeto (faixas e validação) |
| RF10 | #21 | projeto (derivado do Phong) |
| RF11 | #23 | PDF/slides (equação e two-sided); projeto/AGENTS (V de vista, clamp, sem gamma) |
| RF12 | #24, #25 | PDF (tonalização); projeto (interpolação) |
| RF13 | #22, #24 | PDF (z-buffer); projeto/AGENTS (invZ, empate) |
| RF14 | #24 | PDF (scan line); projeto/ADR-0005 (cobertura); projeto/AGENTS/ADR-0009 (invZ) |
| RF15 | #26 | PDF (recarga); projeto (caminhos, tecla, atomicidade) |
| RF16 | #26, #27 | projeto/regressão da 1ª VA |
| RNF04 | #30 | PDF (pixel/bibliotecas); projeto/1VA (C++17); projeto/ADR-0001 (SDL2) |
| RNF05 | #27 | projeto/ADR-0003 |
| RNF06 | #27 | projeto |

## 6. Fora de escopo

- Flat shading e Gouraud shading.
- Matrizes 4×4, transformação de modelo, texturas e iluminação global.
- Múltiplas luzes, atenuação, sombras, transparência e antialiasing.
- Clipping contra o plano próximo.

## 7. Decisões de implementação (não são requisitos)

Estas decisões estão registradas em `docs/especificacao-2va.md`; a ADR-0007 é criada nesta issue, a ADR-0009 em #24, e as ADRs 0008 e 0010 em #29. Elas não devem ser lidas como exigências do PDF.

- **Somente Phong shading.** Flat e Gouraud ficam fora do escopo desta entrega (ADR-0007).
- **Dois arquivos de configuração.** Câmera e iluminação são carregadas de arquivos separados, com caminhos padrão definidos pelo projeto e substituíveis na linha de comando. A câmera é única e compartilhada (ADR-0006).
- **Arquivos padrão e calibração.** A existência, o conteúdo e a calibração de `lighting/lighting.txt` e `lighting/lighting-pdf.txt` são entregues pela issue #20; o requisito apenas exige carregar o arquivo corrente.
- **Redraw event-driven.** O redesenho ocorre em resposta a eventos, sem laço de animação contínuo (ADR-0001).
- **SDL2 só para janela e pixel.** A biblioteca externa permitida é exclusivamente a função de desenho de pixel, conforme ADR-0001.
- **Luz pontual única.** A fonte de luz é pontual, em coordenadas de mundo, sem atenuação.
- **Z-buffer e scan line com atributos.** A especialização do scan line para atributos e a profundidade por `invZ` serão formalizadas na ADR-0009 (#24), complementando a ADR-0005.
