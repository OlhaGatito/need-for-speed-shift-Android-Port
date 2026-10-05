# 🏎️ Need for Speed Shift — Android → Linux ARM

> Port experimental de **Need for Speed Shift (Android)** para handhelds Linux (ARMv7 hard‑float), usando runtime **Marmalade/S3E** e suporte à integração com **PortMaster**.

---  

## ⚡ Estado atual

| 🚦 | Condição |
|----|----------|
| 🧪 | **Em desenvolvimento** – compilável, mas gameplay não confirmado em hardware real. |
| 📅 | Última atualização: **2026‑10‑05** |

---  

## 📦 Como usar (usuário final)

> ⚠️ Nenhum arquivo proprietário (APK, OBB, assets) está neste repositório.  
> Você deve copiar os arquivos do seu próprio backup ou cópia legal do jogo.

### 1️⃣　Preparação
- Copie seu **APK** e **OBB** do jogo para a pasta `data/`.
- Execute o script de configuração:
  ```bash
  ./setup.sh
  ```
- Ele prepara os artefatos e testa o *payload*.

### 2️⃣　Execução
- Execute o launcher com:
  ```bash
  ./run.sh
  ```
- Se necessário, use alternativas:
  ```bash
  ./run-fallback.sh      # fallback caso o run.sh falhe
  ./port_compat.sh      # compatibilidade com CFWs específicos
  ```

---  

## 🗂️ Estrutura do repositório

| 📁 Pasta | Conteúdo |
|---------|----------|
| `src/` | Loader e runtime Marmalade/S3E (arquivo, imagem, vídeo, áudio, input) |
| `include/` | Interfaces e cabeçalhos do projeto |
| `Makefile` | Compilação (ARMv7‑A / hard‑float) |
| `nfsshift_s3e_loader` | Binário do loader compilado |
| `port.json` | Metadata do port (versão, pacote, ABI) |
| `data/` | **Coloque aqui seu APK e OBB!** |
| `run*.sh` | Scripts de execução (com logs em `logs/`) |

---  

## 🛠️ Configuração gráfica

Os scripts tentam usar a stack gráfica nativa do seu dispositivo.  
Se precisar de ajustes, pode definir variáveis antes de rodar:

```bash
export SDL_VIDEO_WIDTH=640
export SDL_VIDEO_HEIGHT=480
export NFSSHIFT_W=640
export NFSSHIFT_H=480
export LIBGL_ES=2
export LIBGL_GL=21
export LIBGL_FB=1
./run.sh
```

> Isso só é necessário se o CFW não estiver configurado corretamente.

---  

## 🔍 Como testar

```bash
# Verifique se o loader tem a arquitetura correta:
ls -l nfsshift_s3e_loader
file nfsshift_s3e_loader
readelf -h nfsshift_s3e_loader

# Verifique dependências no dispositivo (ARM):
ldd nfsshift_s3e_loader
```

> Espera‑se que seja **ARM32/armhf**, não AArch64.

---  

## 📚 Documentação detalhada (centralizada)

Veja a documentação técnica, incluindo método NextOS, logs, teste QEMU e lições em:
- **`Main/docs/NEED-FOR-SPEED-SHIFT.md`**
- **`Main/docs/CHECKLIST.md`**
- **`Main/docs/COMPATIBILITY-REPORT-TEMPLATE.md`**

---  

## 🧑‍💻 Como contribuir

1. **Fork** do repositório **Main** (código‑fonte).  
2. Implemente melhorias nos scripts, correções no loader ou ajustes de compatibilidade.  
3. Abra *pull‑request* com descrição clara dos testes realizados.  
4. Se o CI (GitHub Actions) validar, o port será atualizado aqui.

---  

## ⚖️ Licença

Código‑fonte sob **GPL‑2.0‑or‑later** (não cobre os direitos do jogo).  
Consulte `LICENSE`.

---  

### 🎉 Ready to race! 🏁

```
