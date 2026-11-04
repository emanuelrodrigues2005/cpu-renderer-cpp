# Câmera e iluminação em arquivos separados

## Contexto

O projeto carrega câmera e iluminação de arquivos texto e permite recarregá-los em runtime sem reiniciar a aplicação. A câmera é única e compartilhada entre todas as malhas ([ADR-0006](0006-camera-unica-compartilhada.md)); a iluminação é o modelo de Phong com uma luz pontual e um material ([ADR-0007](0007-modelo-iluminacao-phong.md)). O enunciado mostra um exemplo de iluminação com `Pl = 60 5 -10` que não é calibrado para os modelos e a câmera do projeto. A carga de cada arquivo segue o padrão atômico da [ADR-0002](0002-fronteira-carregamento-byu.md): parsear em cópia local e só substituir o estado após sucesso.

## Decisão

- **Dois arquivos:** a câmera vem de `camera/camera.txt` e a iluminação de `lighting/lighting.txt`. São arquivos separados, com formatos e validadores próprios.
- **CLI:** a aplicação aceita até três argumentos posicionais, nesta ordem: `[modelo.byu] [camera.txt] [lighting.txt]`. Argumentos omitidos ao final usam os padrões `models/piramide.byu`, `camera/camera.txt` e `lighting/lighting.txt`; um quarto posicional é erro.
- **Padrão calibrado e exemplo do PDF:** `lighting/lighting.txt` é o arquivo padrão, calibrado para os modelos e a câmera compartilhada. `lighting/lighting-pdf.txt` preserva o exemplo exato do PDF (luz em `Pl = 60 5 -10`) e **não** é o padrão.
- **Recarga:** `R` recarrega câmera e iluminação a partir dos caminhos correntes (padrões ou posicionais da CLI). Cada fonte é atômica: erro em uma preserva o último valor válido daquela fonte, reporta no `stderr` e não invalida a outra. Se ao menos uma fonte recarregar com sucesso, a cena é redesenhada.
- **Troca de malha:** `1`–`6` troca apenas a malha; câmera e iluminação são compartilhadas e não mudam. O parser de iluminação continua estrito: os oito campos são obrigatórios, sem duplicatas nem tokens extras, com valores finitos e nas faixas.

## Alternativas

- **Arquivo único de configuração:** reduziria o número de arquivos, mas misturaria formatos e faria um erro de iluminação poder invalidar a câmera (ou vice-versa); a separação mantém a atomicidade por fonte.
- **Presets por malha:** contraria a decisão de câmera compartilhada da [ADR-0006](0006-camera-unica-compartilhada.md) e o contrato de que `1`–`6` só troca a malha.
- **Recarregar apenas a câmera:** deixaria a iluminação antiga após editar `lighting/lighting.txt`; `R` deve recarregar ambas as fontes.
- **Usar o exemplo do PDF como padrão:** o exemplo não é calibrado para a câmera e os modelos do projeto; o padrão calibrado evita renders escuros ou com aparência inesperada.

## Consequências

- O usuário edita dois arquivos separados e usa `R` para aplicar ambos sem reiniciar.
- A falha em uma fonte não derruba a outra: cada uma preserva seu último valor válido e reporta o erro no `stderr`.
- O padrão `lighting/lighting.txt` é calibrado para a cena; `lighting/lighting-pdf.txt` fica como referência do enunciado.
- A câmera continua única e compartilhada; a troca de malha não recarrega nem troca configuração.
- O comportamento é coberto por `lighting_policy`, `lighting_padrao`, `lighting_sucesso`, `lighting_invalido`, `lighting_inexistente`, `smoke_iluminacao_invalida` e `camera_padrao` (ver [tests/check_lighting_policy.cmake](../../tests/check_lighting_policy.cmake), [tests/check_default_lighting.cmake](../../tests/check_default_lighting.cmake) e [tests/check_default_camera.cmake](../../tests/check_default_camera.cmake)).
- A implementação está em [src/main.cpp](../../src/main.cpp) e [src/shading/Lighting.cpp](../../src/shading/Lighting.cpp); os arquivos de configuração estão em [lighting/](../../lighting/).
