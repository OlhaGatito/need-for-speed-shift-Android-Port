# Gatito Extractor — Need for Speed Shift

Esta adaptação usa o Gatito Extractor como camada de preparação e validação do payload Marmalade.

## Fluxo

1. A UI Gatito inicia antes do loader.
2. Valida game/NFSShift.s3e.unpacked.
3. Valida game/common.dz.
4. Valida game/gfx.dz.
5. Só depois permite o runtime continuar.

### Limitação atual

O port ainda não possui um unpacker S3E validado para transformar automaticamente o NFSShift.s3e fornecido pelo usuário em NFSShift.s3e.unpacked. Por isso a ferramenta não finge que a extração está completa: ela para exatamente nessa etapa e informa o motivo.

Quando o unpacker for validado, esta mesma UI poderá receber a etapa de extração sem alterar o fluxo de validação.

Os dados do jogo continuam BYO-data e não entram no repositório.
