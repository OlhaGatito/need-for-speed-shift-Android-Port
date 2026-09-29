#!/bin/bash
# Need for Speed Shift — internal PortMaster runtime

set +u

GAMEDIR="$(CDPATH= cd -- "$(dirname -- "$0")" 2>/dev/null && pwd -P)" || exit 1
cd "$GAMEDIR" || exit 1

LOGDIR="${NFSSHIFT_LOG_DIR:-$GAMEDIR/logs}"
mkdir -p "$LOGDIR" 2>/dev/null || exit 1
LOG="${NFSSHIFT_LOG:-$LOGDIR/debug.log}"
touch "$LOG" 2>/dev/null || LOG="${TMPDIR:-/tmp}/nfsshift-debug.log"
exec >>"$LOG" 2>&1

echo "=== Need for Speed Shift internal runtime ==="
echo "GAMEDIR=$GAMEDIR"
echo "CFW=${CFW_NAME:-unknown}"
echo "DEVICE=${DEVICE_NAME:-unknown}"
echo "ARCH=${DEVICE_ARCH:-unknown}"

GAME_DIR="$GAMEDIR/game"
GAME_IMAGE="$GAME_DIR/NFSShift.s3e.unpacked"
LOADER="$GAMEDIR/nfsshift_s3e_loader"

[ -f "$LOADER" ] || { echo "[ERROR] loader not found: $LOADER"; exit 1; }
[ -f "$GAME_IMAGE" ] || { echo "[ERROR] game image not found: $GAME_IMAGE"; exit 1; }
[ -f "$GAME_DIR/common.dz" ] || { echo "[ERROR] common.dz not found"; exit 1; }
[ -f "$GAME_DIR/gfx.dz" ] || { echo "[ERROR] gfx.dz not found"; exit 1; }
chmod +x "$LOADER" 2>/dev/null || true

export PORT_32BIT=Y
unset LD_PRELOAD 2>/dev/null || true

if [ -d "$GAMEDIR/libs.armhf" ]; then
    export LD_LIBRARY_PATH="$GAMEDIR/libs.armhf${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

export SDL_VIDEO_WIDTH="${SDL_VIDEO_WIDTH:-${DISPLAY_WIDTH:-640}}"
export SDL_VIDEO_HEIGHT="${SDL_VIDEO_HEIGHT:-${DISPLAY_HEIGHT:-480}}"
export NFSSHIFT_W="${NFSSHIFT_W:-${DISPLAY_WIDTH:-640}}"
export NFSSHIFT_H="${NFSSHIFT_H:-${DISPLAY_HEIGHT:-480}}"

export LIBGL_ES="${LIBGL_ES:-2}"
export LIBGL_GL="${LIBGL_GL:-21}"
export LIBGL_FB="${LIBGL_FB:-1}"

export SDL_GAMECONTROLLERCONFIG="${sdl_controllerconfig:-${SDL_GAMECONTROLLERCONFIG:-}}"

echo "--- AUDIO DETECTION ---"
if [ -d /dev/snd ]; then
    echo "audio: /dev/snd available"
    ls -la /dev/snd 2>/dev/null || true
else
    echo "audio: /dev/snd unavailable"
fi
[ -f /proc/asound/cards ] && cat /proc/asound/cards
command -v pactl >/dev/null 2>&1 && echo "audio: PulseAudio tools available"
command -v pw-cli >/dev/null 2>&1 && echo "audio: PipeWire tools available"
echo "SDL_AUDIODRIVER=${SDL_AUDIODRIVER:-auto}"

echo "--- VIDEO DETECTION ---"
[ -d /dev/dri ] && ls -la /dev/dri 2>/dev/null || true
[ -e /dev/fb0 ] && echo "video: framebuffer available"
[ -e /dev/mali0 ] && echo "video: Mali device available"
echo "SDL_VIDEODRIVER=${SDL_VIDEODRIVER:-auto}"

if [ -n "${XDG_RUNTIME_DIR:-}" ]; then
    :
elif [ -d "/run/user/$(id -u 2>/dev/null)" ]; then
    export XDG_RUNTIME_DIR="/run/user/$(id -u)"
else
    export XDG_RUNTIME_DIR="/tmp/runtime-$(id -u)"
    mkdir -p "$XDG_RUNTIME_DIR" 2>/dev/null || true
fi

if command -v pm_platform_helper >/dev/null 2>&1; then
    pm_platform_helper "$LOADER" || echo "[WARN] pm_platform_helper returned $?"
fi

if [ -f "$GAMEDIR/nfsshift.gptk" ] && [ -n "${GPTOKEYB:-}" ]; then
    "$GPTOKEYB" "$LOADER" -c "$GAMEDIR/nfsshift.gptk" &
    GPTOKEYB_PID=$!
else
    GPTOKEYB_PID=""
fi

echo "--- STARTING LOADER ---"
"$LOADER" --run --root "$GAME_DIR" "$GAME_IMAGE"
GAME_RC=$?

echo "Loader exit code: $GAME_RC"

if [ "$GAME_RC" -ne 0 ] && [ "${NFSSHIFT_FALLBACK:-0}" != "1" ] && [ -x "$GAMEDIR/run-fallback.sh" ]; then
    echo "--- STARTING COMPATIBILITY FALLBACK ---"
    export NFSSHIFT_FALLBACK=1
    "$GAMEDIR/run-fallback.sh"
    GAME_RC=$?
    echo "Fallback exit code: $GAME_RC"
fi

if [ -n "${GPTOKEYB_PID:-}" ]; then
    kill "$GPTOKEYB_PID" 2>/dev/null || true
fi

if command -v pidof >/dev/null 2>&1 && [ -n "${ESUDO:-}" ]; then
    $ESUDO kill -9 "$(pidof gptokeyb)" 2>/dev/null || true
fi

unset LD_PRELOAD
unset SDL_GAMECONTROLLERCONFIG
pm_finish 2>/dev/null || true
exit "$GAME_RC"
