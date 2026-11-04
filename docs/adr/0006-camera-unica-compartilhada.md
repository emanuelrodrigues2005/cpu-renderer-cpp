# Câmera única compartilhada em `camera/camera.txt`

Por instrução definitiva do professor, todos os modelos passam a compartilhar o mesmo arquivo de câmera, `camera/camera.txt`, substituindo os presets calibrados por malha (`camera/presets/*.txt`) e o comportamento da #13 (em que `R` recarregava o preset do modelo corrente). O app mantém uma única `Camera` em memória: as teclas `1`–`6` trocam apenas a malha e `R` relê o caminho de câmera corrente (padrão `camera/camera.txt`, ou o 2º argumento posicional quando informado). A carga continua atômica (ADR-0002): arquivo inválido preserva a última câmera válida e reporta no `stderr`.

## Consequências

- `camera/presets/` deixa de existir; os valores calibrados por modelo saem da árvore (ficam no histórico do git).
- Trocar de modelo não altera a câmera; após editar o arquivo e apertar `R` vendo um objeto, qualquer objeto seguinte usa a configuração nova (câmera global, enquadramento variável entre modelos).
- `tests/check_default_camera.cmake` compara o dump padrão com o dump explícito de `camera/camera.txt`; `tests/check_camera_policy.cmake` impede a volta de presets em `src/` ou da pasta.
- O conteúdo de `camera/camera.txt` permanece o exemplo do PDF.
