# Need for Speed Shift — s3e Port

Port experimental de **Need for Speed Shift (Android)** para sistemas Linux ARM32/armhf compatíveis com **PortMaster**.

> **Estado:** funcional como port/loader em desenvolvimento. O projeto não distribui os arquivos proprietários do jogo.

## Visão geral

A versão Android de Need for Speed Shift utiliza a camada/runtime **s3e**. O objetivo deste projeto é fornecer no Linux as interfaces necessárias para executar a imagem do jogo através de um loader próprio.

## Runtime s3e

O loader deste projeto implementa uma camada de compatibilidade para as chamadas necessárias do runtime, permitindo executar a imagem `NFSShift.s3e.unpacked` em um ambiente Linux ARM32.

Entre as interfaces implementadas no projeto estão:

- `s3e_file` — acesso a arquivos;
- `s3e_config` — configuração/runtime;
- `s3e_input` — entrada e controles;
- `s3e_audio` — áudio;
- `s3e_image` — imagens;
- `s3e_gl` — OpenGL/OpenGL ES;
- `s3e_runtime` — runtime;
- `s3e_host` — integração com o host Linux;
- `derbh.c` — suporte auxiliar utilizado pelo loader.

## Estrutura do projeto

```
.
├── Makefile
├── run.sh
├── port.json
├── nfsshift_s3e_loader
├── include/
├── src/
│   ├── derbh.c
│   ├── main.c
│   ├── s3e_audio.c
│   ├── s3e_config.c
│   ├── s3e_file.c
│   ├── s3e_gl.c
│   ├── s3e_host.c
│   ├── s3e_image.c
│   ├── s3e_input.c
│   └── s3e_runtime.c
└── third_party/
    └── lzma/
        └── LzmaDec.c
```

Os arquivos proprietários do jogo devem ser fornecidos pelo usuário a partir da cópia legítima do jogo.

## Loader s3e

O executável principal do port é:

```
nfsshift_s3e_loader
```

Ele recebe a imagem:

```
NFSShift.s3e.unpacked
```

e inicia a aplicação através da camada de compatibilidade implementada em `src/`.

A execução utiliza:

```text
--run
--root <diretório-do-jogo>
<NFSShift.s3e.unpacked>
```

O diretório raiz permite que as chamadas de arquivo encontrem `common.dz`, `gfx.dz` e os demais recursos sem depender de caminhos absolutos.


## Ambiente gráfico

O port utiliza o ambiente gráfico disponibilizado pelo sistema, sendo compatível com ambientes que suportam **PortMaster**, sem substituir à força as bibliotecas gráficas do sistema.

Os valores padrão utilizados pelo launcher são:

```
SDL_VIDEO_WIDTH=640
SDL_VIDEO_HEIGHT=480

NFSSHIFT_W=640
NFSSHIFT_H=480

LIBGL_ES=2
LIBGL_GL=21
LIBGL_FB=1
```

Esses valores podem ser sobrescritos pelo ambiente antes da execução.

A intenção é preservar a configuração gráfica do sistema sempre que possível.

## Áudio

A camada:

```
src/s3e_audio.c
```

é responsável pela compatibilidade das chamadas de áudio esperadas pelo runtime.

O launcher não força uma implementação de áudio específica, permitindo utilizar o ambiente de áudio disponibilizado pelo sistema.

## Controles

A camada:

```
src/s3e_input.c
```

faz a tradução da entrada do sistema para a API esperada pelo jogo.


## Diagnóstico

### Loader não encontrado

Verifique:

```bash
ls -l nfsshift_s3e_loader
chmod +x nfsshift_s3e_loader
```

### Verificar arquitetura

```bash
file nfsshift_s3e_loader
readelf -h nfsshift_s3e_loader
```

Se aparecer **AArch64/ARM64**, o binário foi compilado para a arquitetura errada.

### Verificar bibliotecas

```bash
ldd nfsshift_s3e_loader
```

Em um ambiente ARM diferente do host, `ldd` deve ser executado no próprio dispositivo ou substituído por análise de `readelf -d`.

## Arquivos proprietários

Este repositório não deve distribuir:

- APK original;
- OBB original;
- assets proprietários;
- arquivos de áudio proprietários;
- texturas proprietárias;
- executável original do jogo.

O usuário deve obter os arquivos do jogo por meios legítimos.

## Licença

Consulte o arquivo [LICENSE](LICENSE) deste repositório para a licença aplicável ao código publicado.

Os arquivos e marcas pertencentes à Electronic Arts e aos demais detentores dos direitos de Need for Speed Shift não fazem parte da licença do código deste projeto.

## Créditos técnicos

Projeto focado em engenharia reversa e compatibilidade de runtime para Linux ARM32.

Componentes principais:

- s3e — runtime alvo;
- ARMv7-A / armhf — arquitetura do port;
- OpenGL ES — camada gráfica;
- PortMaster — ambiente de execução compatível;
- GCC ARM GNU/Linux — ferramenta de compilação.

## Observação sobre `port.json`

O arquivo `port.json` existente no repositório ainda contém o campo:

```json
"engine": "Unity"
```

Esse valor é inconsistente com a engenharia reversa realizada neste projeto.

O README documenta a arquitetura efetivamente utilizada pelo port. O campo de metadata pode ser corrigido posteriormente.
