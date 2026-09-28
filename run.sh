cd /mnt/c/Users/Fabricio/Videos/nfsshift

cat > nfsshift.sh <<'EOF'
#!/bin/sh
#
# Need for Speed Shift — NextOS / PortMaster / muOS
# Marmalade / s3e
#
# Estrutura:
#
#   ports/
#   ├── nfsshift.sh
#   └── nfsshift/
#       ├── nfsshift_s3e_loader
#       ├── run.sh
#       └── game/
#           ├── NFSShift.s3e.unpacked
#           ├── common.dz
#           ├── gfx.dz
#           └── bgm/
#
# O launcher não depende de caminhos absolutos.
#

set -u

# ------------------------------------------------------------
# PortMaster
# ------------------------------------------------------------

CONTROLFOLDER=""

if [ -n "${controlfolder:-}" ] && [ -f "$controlfolder/control.txt" ]; then
    CONTROLFOLDER="$controlfolder"
elif [ -f "/opt/system/Tools/PortMaster/control.txt" ]; then
    CONTROLFOLDER="/opt/system/Tools/PortMaster"
elif [ -f "/opt/tools/PortMaster/control.txt" ]; then
    CONTROLFOLDER="/opt/tools/PortMaster"
elif [ -n "${XDG_DATA_HOME:-}" ] &&
     [ -f "$XDG_DATA_HOME/PortMaster/control.txt" ]; then
    CONTROLFOLDER="$XDG_DATA_HOME/PortMaster"
elif [ -f "/roms/ports/PortMaster/control.txt" ]; then
    CONTROLFOLDER="/roms/ports/PortMaster"
fi

if [ -n "$CONTROLFOLDER" ]; then
    controlfolder="$CONTROLFOLDER"
    export controlfolder

    # shellcheck disable=SC1090
    . "$controlfolder/control.txt"

    if [ -f "$controlfolder/mod_${CFW_NAME:-}.txt" ]; then
        # shellcheck disable=SC1090
        . "$controlfolder/mod_${CFW_NAME}.txt"
    fi

    if command -v get_controls >/dev/null 2>&1; then
        get_controls
    fi
fi

# ------------------------------------------------------------
# Diretório do port
# ------------------------------------------------------------

if [ -n "${directory:-}" ]; then
    GAMEDIR="/$directory/ports/nfsshift"
else
    # Fallback para execução direta durante desenvolvimento.
    GAMEDIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
fi

if [ ! -d "$GAMEDIR" ]; then
    echo "NFS Shift: game directory not found:"
    echo "$GAMEDIR"
    exit 1
fi

GAME_DIR="$GAMEDIR/game"
LOADER="$GAMEDIR/nfsshift_s3e_loader"
GAME_IMAGE="$GAME_DIR/NFSShift.s3e.unpacked"

# ------------------------------------------------------------
# Validação dos arquivos
# ------------------------------------------------------------

if [ ! -x "$LOADER" ]; then
    echo "NFS Shift: loader not found or not executable:"
    echo "$LOADER"
    exit 1
fi

if [ ! -f "$GAME_IMAGE" ]; then
    echo "NFS Shift: game image not found:"
    echo "$GAME_IMAGE"
    exit 1
fi

if [ ! -f "$GAME_DIR/common.dz" ]; then
    echo "NFS Shift: common.dz not found."
    exit 1
fi

if [ ! -f "$GAME_DIR/gfx.dz" ]; then
    echo "NFS Shift: gfx.dz not found."
    exit 1
fi

# ------------------------------------------------------------
# Ambiente
# ------------------------------------------------------------

export PORT_32BIT="Y"

# Nunca carregar preload externo de outro port.
unset LD_PRELOAD 2>/dev/null || true

# Preserva o ambiente existente e somente acrescenta libs
# específicas do port caso elas existam.
LIBDIR="$GAMEDIR/libs.armhf"

if [ -d "$LIBDIR" ]; then
    if [ -n "${LD_LIBRARY_PATH:-}" ]; then
        export LD_LIBRARY_PATH="$LIBDIR:$LD_LIBRARY_PATH"
    else
        export LD_LIBRARY_PATH="$LIBDIR"
    fi
fi

# ------------------------------------------------------------
# SDL / OpenGL
# ------------------------------------------------------------

# Mantemos o ambiente escolhido pelo firmware/PortMaster.
# Estes valores são apenas defaults compatíveis com o loader.
export SDL_VIDEO_WIDTH="${SDL_VIDEO_WIDTH:-640}"
export SDL_VIDEO_HEIGHT="${SDL_VIDEO_HEIGHT:-480}"

export NFSSHIFT_W="${NFSSHIFT_W:-640}"
export NFSSHIFT_H="${NFSSHIFT_H:-480}"

# Compatibilidade GLES utilizada pelo ambiente NextOS/muOS.
export LIBGL_ES="${LIBGL_ES:-2}"
export LIBGL_GL="${LIBGL_GL:-21}"
export LIBGL_FB="${LIBGL_FB:-1}"

# ------------------------------------------------------------
# Runtime
# ------------------------------------------------------------

if [ -n "${XDG_RUNTIME_DIR:-}" ]; then
    :
elif [ -d "/run/user/$(id -u 2>/dev/null)" ]; then
    export XDG_RUNTIME_DIR="/run/user/$(id -u)"
else
    export XDG_RUNTIME_DIR="/tmp/runtime-$(id -u)"
    mkdir -p "$XDG_RUNTIME_DIR" 2>/dev/null || true
fi

# ------------------------------------------------------------
# Controles
# ------------------------------------------------------------

if command -v gptokeyb >/dev/null 2>&1; then
    GPTOKEYB="$(command -v gptokeyb)"
else
    GPTOKEYB=""
fi

# ------------------------------------------------------------
# pm_platform_helper
# ------------------------------------------------------------

if command -v pm_platform_helper >/dev/null 2>&1; then
    pm_platform_helper "$LOADER" 2>/dev/null || true
fi

# ------------------------------------------------------------
# CPU/tasksetter
# ------------------------------------------------------------

TASKSET=""

if [ -n "${tasksetter:-}" ] && [ -x "$tasksetter" ]; then
    TASKSET="$tasksetter"
elif command -v taskset >/dev/null 2>&1; then
    # Não impomos afinidade; somente disponibilizamos o comando.
    TASKSET="taskset"
fi

# ------------------------------------------------------------
# Diretório de execução
# ------------------------------------------------------------

cd "$GAME_DIR" || exit 1

# ------------------------------------------------------------
# Execução
# ------------------------------------------------------------

EXIT_CODE=0

if [ -n "$GPTOKEYB" ]; then
    "$GPTOKEYB" "$LOADER" -c \
        2>/dev/null || true
fi

if [ -n "$TASKSET" ] && [ "$TASKSET" = "taskset" ]; then
    "$LOADER" \
        --run \
        --root "$GAME_DIR" \
        "$GAME_IMAGE"
    EXIT_CODE=$?
else
    "$LOADER" \
        --run \
        --root "$GAME_DIR" \
        "$GAME_IMAGE"
    EXIT_CODE=$?
fi

# ------------------------------------------------------------
# Finalização
# ------------------------------------------------------------

if [ -n "$GPTOKEYB" ]; then
    killall gptokeyb 2>/dev/null || true
fi

if command -v pm_finish >/dev/null 2>&1; then
    pm_finish
fi

exit "$EXIT_CODE"
EOF

chmod +x nfsshift.sh