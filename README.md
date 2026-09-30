# Need for Speed Shift — port Android para Linux ARM

Port experimental da versão Android de **Need for Speed Shift** para Linux ARM32/armhf, com compatibilidade Marmalade/S3E e integração de launcher para PortMaster.

> **Estado:** em desenvolvimento. O repositório contém loader, camadas de compatibilidade e scripts de execução; isso não significa suporte completo a todas as funções ou dispositivos.

## Visão geral

| Item | Alvo registrado |
|---|---|
| Jogo de origem | Need for Speed Shift para Android |
| Runtime | Marmalade S3E |
| Arquitetura | ARMv7-A / Linux armhf |
| Execução | Linux ARM com ambiente PortMaster compatível |
| Gráficos | OpenGL ES, usando a configuração oferecida pelo sistema |

## Mapa dos componentes

| Componente | Conteúdo e função |
|:--|:--|
| 🧩 **Runtime S3E**<br>`src/` | Loader e camadas para runtime, arquivos, configuração, imagem, gráficos, áudio e controles. |
| 📎 **Interfaces**<br>`include/` | Cabeçalhos e interfaces do projeto. |
| 🛠️ **Build**<br>`Makefile` | Regras de compilação. |
| 🚀 **Execução**<br>`run.sh`<br>`run-fallback.sh`<br>`port_compat.sh` | Scripts de execução e caminhos de compatibilidade. |
| 🎮 **Launcher**<br>`Need for Speed Shift.sh` | Entrada PortMaster do port. |
| 📦 **Loader**<br>`nfsshift_s3e_loader` | Artefato do loader presente no repositório. |

## Configuração gráfica padrão

O launcher define os valores abaixo como ponto de partida. O ambiente pode sobrescrevê-los antes de iniciar o port.

```text
SDL_VIDEO_WIDTH=640
SDL_VIDEO_HEIGHT=480
NFSSHIFT_W=640
NFSSHIFT_H=480
LIBGL_ES=2
LIBGL_GL=21
LIBGL_FB=1
```

A configuração procura usar a stack gráfica existente no sistema, sem substituir à força as bibliotecas fornecidas pelo CFW.

## Diagnóstico do loader

Confira o arquivo e a arquitetura compilada:

```bash
ls -l nfsshift_s3e_loader
file nfsshift_s3e_loader
readelf -h nfsshift_s3e_loader
```

O alvo esperado é ARM32/armhf, não AArch64. Em um dispositivo ARM diferente do computador de desenvolvimento, execute `ldd` no próprio dispositivo ou examine dependências ELF com `readelf -d`.

## Dados do jogo

Este repositório não distribui APK, OBB, assets, bibliotecas ou executável proprietário. Os arquivos necessários devem vir da cópia legítima do próprio usuário e permanecer fora deste repositório.

## Licença e marcas

A licença do código publicado está em [LICENSE](LICENSE). Ela não cobre os arquivos ou marcas de Need for Speed Shift nem materiais de seus detentores de direitos.
