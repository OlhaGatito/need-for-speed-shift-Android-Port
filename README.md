# Need for Speed Shift — Marmalade / s3e Port

Port experimental de **Need for Speed Shift (Android)** para sistemas Linux ARM32/armhf, com foco em dispositivos portáteis que utilizam **PortMaster**, **NextOS** ou **muOS**.

> **Estado:** funcional como port/loader em desenvolvimento. O projeto não distribui os arquivos proprietários do jogo.

## Visão geral

A versão Android de Need for Speed Shift foi construída sobre a **Marmalade SDK**, utilizando a camada/runtime **s3e**. O objetivo deste projeto é reproduzir no Linux as interfaces que o executável Marmalade espera e carregar a imagem do jogo através de um loader próprio.

A arquitetura do port é:

```
Need for Speed Shift
        │
        ▼
NFSShift.s3e.unpacked
        │
        ▼
nfsshift_s3e_loader
        │
        ├── s3e runtime
        ├── s3e_file
        ├── s3e_config
        ├── s3e_input
        ├── s3e_audio
        ├── s3e_image
        ├── s3e_gl
        └── s3e_host
        │
        ▼
Linux ARM32 / armhf
        │
        ▼
OpenGL ES / SDL / PortMaster
```

## Engine utilizada

### Marmalade SDK / s3e

Este jogo **não é um jogo Unity**. A versão Android analisada utiliza o ecossistema **Marmalade**, cujo runtime de aplicação é baseado nas APIs `s3e`.

O loader deste projeto implementa uma camada de compatibilidade para as chamadas necessárias do runtime, permitindo executar a imagem `NFSShift.s3e.unpacked` em um ambiente Linux ARM32.

Entre as interfaces implementadas no projeto estão:

- `s3e_file` — acesso a arquivos;
- `s3e_config` — configuração/runtime;
- `s3e_input` — entrada e controles;
- `s3e_audio` — áudio;
- `s3e_image` — imagens;
- `s3e_gl` — OpenGL/OpenGL ES;
- `s3e_runtime` — runtime Marmalade;
- `s3e_host` — integração com o host Linux;
- `derbh.c` — suporte auxiliar utilizado pelo loader;
- `third_party/lzma/LzmaDec.c` — decodificação LZMA.

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

## Arquivos necessários do jogo

O launcher espera a seguinte estrutura:

```
ports/
├── nfsshift.sh
└── nfsshift/
    ├── nfsshift_s3e_loader
    ├── run.sh
    └── game/
        ├── NFSShift.s3e.unpacked
        ├── common.dz
        ├── gfx.dz
        └── bgm/
```

Os arquivos `.dz` fazem parte dos dados do jogo e são utilizados pelo runtime durante a execução.

## Loader s3e

O executável principal do port é:

```
nfsshift_s3e_loader
```

Ele recebe a imagem:

```
NFSShift.s3e.unpacked
```

e inicia a aplicação Marmalade através da camada de compatibilidade implementada em `src/`.

A execução utiliza:

```text
--run
--root <diretório-do-jogo>
<NFSShift.s3e.unpacked>
```

O diretório raiz é importante porque permite que as chamadas de arquivo do runtime encontrem `common.dz`, `gfx.dz` e os demais recursos sem depender de caminhos absolutos.

## LZMA / arquivos comprimidos

O projeto inclui o decoder:

```
third_party/lzma/LzmaDec.c
```

Ele é compilado diretamente junto ao loader.

A presença dessa implementação é importante para o runtime lidar com dados comprimidos utilizados pelo jogo e/ou pelo formato Marmalade analisado durante o processo de porting.

## Arquitetura de CPU

O alvo atual é:

- **ARMv7-A**
- **ARM32**
- **EABI**
- **hard-float / armhf**
- **NEON**
- **VFPv4**

As flags utilizadas pelo Makefile são:

```
-march=armv7-a
-mfpu=neon-vfpv4
-mfloat-abi=hard
```

Portanto, o binário gerado não é ARM64/AArch64.

## Compilação

O compilador padrão esperado pelo Makefile é:

```
arm-linux-gnueabihf-gcc
```

Compilação simples:

```bash
make
```

Ou especificando explicitamente o cross-compiler:

```bash
make CC=arm-linux-gnueabihf-gcc
```

Também é possível apontar para um toolchain específico:

```bash
make CC=/caminho/para/arm-linux-gnueabihf-gcc
```

### Flags completas

O Makefile utiliza atualmente:

```
-std=c11
-D_GNU_SOURCE
-Wall
-Iinclude
-Ithird_party
-Ithird_party/lzma
-march=armv7-a
-mfpu=neon-vfpv4
-mfloat-abi=hard
```

Bibliotecas de sistema:

```
-ldl
-pthread
-lm
```

## Fontes compilados

O loader é construído a partir de:

```
src/derbh.c
src/main.c
src/s3e_audio.c
src/s3e_config.c
src/s3e_file.c
src/s3e_gl.c
src/s3e_host.c
src/s3e_image.c
src/s3e_input.c
src/s3e_runtime.c
third_party/lzma/LzmaDec.c
```

O comando final é equivalente a:

```bash
arm-linux-gnueabihf-gcc \
  -O2 \
  -std=c11 \
  -D_GNU_SOURCE \
  -Wall \
  -Iinclude \
  -Ithird_party \
  -Ithird_party/lzma \
  -march=armv7-a \
  -mfpu=neon-vfpv4 \
  -mfloat-abi=hard \
  -o nfsshift_s3e_loader \
  src/derbh.c \
  src/main.c \
  src/s3e_audio.c \
  src/s3e_config.c \
  src/s3e_file.c \
  src/s3e_gl.c \
  src/s3e_host.c \
  src/s3e_image.c \
  src/s3e_input.c \
  src/s3e_runtime.c \
  third_party/lzma/LzmaDec.c \
  -ldl -pthread -lm
```

Depois da compilação o Makefile executa:

```bash
arm-linux-gnueabihf-strip -s nfsshift_s3e_loader
```

Isso reduz o tamanho do executável removendo símbolos desnecessários para a execução.

## Verificação do binário

Depois da compilação, recomenda-se verificar:

```bash
file nfsshift_s3e_loader
```

e:

```bash
readelf -h nfsshift_s3e_loader
```

O resultado esperado é um ELF **32-bit ARM** compatível com ARMv7/armhf.

Também é útil verificar as dependências:

```bash
readelf -d nfsshift_s3e_loader
```

## Execução no PortMaster / muOS

O `run.sh` prepara o ambiente para diferentes instalações do PortMaster.

Ele procura o PortMaster em locais como:

```
/opt/system/Tools/PortMaster
/opt/tools/PortMaster
$XDG_DATA_HOME/PortMaster
/roms/ports/PortMaster
```

Quando encontrado, o script carrega:

```
control.txt
```

e, quando disponível, o módulo específico do firmware:

```
mod_<CFW_NAME>.txt
```

O launcher também tenta utilizar:

- `get_controls`;
- `gptokeyb`;
- `pm_platform_helper`;
- `pm_finish`.

## Ambiente gráfico

O port foi preparado para trabalhar com o ambiente gráfico fornecido pelo firmware/PortMaster, sem substituir à força as bibliotecas gráficas do sistema.

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

A intenção é preservar a configuração gráfica do firmware sempre que possível, evitando carregar bibliotecas externas incompatíveis.

## Áudio

A camada:

```
src/s3e_audio.c
```

é responsável pela compatibilidade das chamadas de áudio esperadas pelo runtime Marmalade.

O launcher não força uma implementação de áudio específica do dispositivo. Dessa forma, o port pode utilizar o ambiente de áudio disponibilizado pelo firmware/PortMaster.

## Controles

A camada:

```
src/s3e_input.c
```

faz a tradução da entrada do sistema para a API esperada pelo jogo.

O launcher detecta `gptokeyb` quando disponível e inicializa o ambiente de controles do PortMaster antes de executar o loader.

## Execução manual para testes

Durante o desenvolvimento, o loader pode ser executado diretamente no diretório do jogo:

```bash
cd game
../nfsshift_s3e_loader \
  --run \
  --root "$(pwd)" \
  "$(pwd)/NFSShift.s3e.unpacked"
```

Isso é útil para separar problemas do loader de problemas do launcher/PortMaster.

## Diagnóstico

### Loader não encontrado

Verifique:

```bash
ls -l nfsshift_s3e_loader
chmod +x nfsshift_s3e_loader
```

### Arquivo do jogo não encontrado

Verifique:

```bash
ls -lh game/NFSShift.s3e.unpacked
ls -lh game/common.dz
ls -lh game/gfx.dz
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

## Processo de porting

O port foi desenvolvido por etapas:

1. Identificação do runtime Marmalade/s3e utilizado pelo jogo.
2. Extração e análise da imagem `NFSShift.s3e`.
3. Preparação de `NFSShift.s3e.unpacked`.
4. Implementação das APIs s3e necessárias.
5. Inclusão do decoder LZMA.
6. Adaptação das operações de arquivo para Linux.
7. Adaptação de entrada e controles.
8. Adaptação do áudio.
9. Adaptação da camada gráfica/OpenGL ES.
10. Cross-compilação para ARMv7 hard-float.
11. Integração com PortMaster.
12. Criação do launcher para NextOS/muOS.
13. Testes no hardware ARM alvo.

## Relação com PortMaster

O projeto não é uma implementação alternativa do PortMaster. O PortMaster é utilizado como camada de integração com o firmware, fornecendo recursos como:

- localização do diretório do port;
- configuração de controles;
- helpers de plataforma;
- ambiente de execução;
- finalização/restauração do sistema.

O código específico do jogo permanece no loader e nas implementações s3e deste repositório.

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

Projeto focado em engenharia reversa, compatibilidade de runtime e porting de software Marmalade/s3e para Linux ARM32.

Componentes principais:

- Marmalade / s3e — runtime alvo;
- ARMv7-A / armhf — arquitetura do port;
- OpenGL ES — camada gráfica;
- LZMA SDK decoder — decodificação;
- PortMaster — integração com firmware;
- muOS / NextOS — ambientes de teste;
- GCC ARM GNU/Linux — cross-compilação.

## Observação sobre `port.json`

O arquivo `port.json` existente no repositório ainda contém o campo:

```json
"engine": "Unity"
```

Esse valor é inconsistente com a engenharia reversa realizada neste projeto: **Need for Speed Shift utiliza Marmalade/s3e, não Unity**.

O README documenta a arquitetura efetivamente utilizada pelo port. O campo de metadata pode ser corrigido posteriormente para refletir Marmalade/s3e.
