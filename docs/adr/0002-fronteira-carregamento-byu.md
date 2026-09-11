# Fronteira BYU: validar e normalizar na entrada

O carregador (`Mesh`) é a única fronteira do formato BYU. Ele valida o arquivo inteiro na entrada, converte os índices para 0-based e ignora o conteúdo após o último triângulo. Tolerar sobras é deliberado: `piramide.byu` termina com um segundo objeto sem cabeçalho, e rejeitá-lo quebraria o arquivo do professor; suportar múltiplos objetos criaria um formato que o enunciado não define. A carga falha com `false` e mensagem, sem exceções e sem tocar na malha de destino, para que uma recarga inválida preserve o último estado válido.

## Consequências

- O restante do pipeline confia na malha: nenhum módulo revalida índices nem vê índices 1-based.
- Recargas (troca de modelo em runtime e, adiante, da câmera) seguem o mesmo padrão: parsear em cópia local e só então trocar o estado.
