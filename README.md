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
OpenGL ES / SDL
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

e inicia a aplicação Marmalade através da camada de compatibilidade implementada em `src/`.

A execução utiliza:

```text
--run
--root <diretório-do-jogo>
<NFSShift.s3e.unpacked>
```

O diretório raiz é importante porque permite que as chamadas de arquivo do runtime encontrem `common.dz`, `gfx.dz` e os demais recursos sem depender de caminhos absolutos.


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

A camada de entrada faz a tradução dos controles do sistema para a API esperada pelo jogo.

## Execução manual para testes

Durante o desenvolvimento, o loader pode ser executado diretamente no diretório do jogo:

```bash
cd game
../nfsshift_s3e_loader \\
  --run \\
  --root "$(pwd)" \\
  "$(pwd)/NFSShift.s3e.unpacked"
```

Isso é útil para testes isolados do loader.

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

## Processo de porting

O port foi desenvolvido por etapas:

1. Identificação do runtime Marmalade/s3e utilizado pelo jogo.
2. Extração e análise da imagem `NFSShift.s3e`.
3. Preparação de `NFSShift.s3e.unpacked`.
4. Implementação das APIs s3e necessárias.
6. Adaptação das operações de arquivo para Linux.
7. Adaptação de entrada e controles.
8. Adaptação do áudio.
9. Adaptação da camada gráfica/OpenGL ES.
12. Criação do launcher para os ambientes de teste.
13. Testes no hardware ARM alvo.


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
- - - muOS / NextOS — ambientes de teste;
- GCC ARM GNU/Linux — cross-compilação.

## Observação sobre `port.json`

O arquivo `port.json` existente no repositório ainda contém o campo:

```json
"engine": "Unity"
```

Esse valor é inconsistente com a engenharia reversa realizada neste projeto: **Need for Speed Shift utiliza Marmalade/s3e, não Unity**.

O README documenta a arquitetura efetivamente utilizada pelo port. O campo de metadata pode ser corrigido posteriormente para refletir Marmalade/s3e.
