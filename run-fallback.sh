#!/bin/bash
# Need for Speed Shift — compatibility retry

set +u

GAMEDIR="$(CDPATH= cd -- "$(dirname -- "$0")" 2>/dev/null && pwd -P)" || exit 1
GAME_DIR="$GAMEDIR/game"
GAME_IMAGE="$GAME_DIR/NFSShift.s3e.unpacked"
LOADER="$GAMEDIR/nfsshift_s3e_loader"

LOGDIR="$GAMEDIR/logs"
mkdir -p "$LOGDIR"
exec >>"$LOGDIR/fallback.log" 2>&1

echo "=== Need for Speed Shift fallback ==="

[ -f "$LOADER" ] || exit 1
[ -f "$GAME_IMAGE" ] || exit 1

if [ -d /dev/snd ]; then
    export SDL_AUDIODRIVER="${NFSSHIFT_AUDIO_DRIVER:-alsa}"
    echo "Fallback audio: SDL_AUDIODRIVER=$SDL_AUDIODRIVER"
fi

if [ -e /dev/fb0 ] && [ -z "${SDL_VIDEODRIVER:-}" ]; then
    export SDL_VIDEODRIVER=fbcon
    echo "Fallback video: SDL_VIDEODRIVER=$SDL_VIDEODRIVER"
fi

export SDL_VIDEO_WIDTH="${SDL_VIDEO_WIDTH:-${DISPLAY_WIDTH:-640}}"
export SDL_VIDEO_HEIGHT="${SDL_VIDEO_HEIGHT:-${DISPLAY_HEIGHT:-480}}"
export NFSSHIFT_W="${NFSSHIFT_W:-${DISPLAY_WIDTH:-640}}"
export NFSSHIFT_H="${NFSSHIFT_H:-${DISPLAY_HEIGHT:-480}}"

export LIBGL_ES="${LIBGL_ES:-2}"
export LIBGL_GL="${LIBGL_GL:-21}"
export LIBGL_FB="${LIBGL_FB:-1}"

if [ -d "$GAMEDIR/libs.armhf" ]; then
    export LD_LIBRARY_PATH="$GAMEDIR/libs.armhf${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

command -v pm_platform_helper >/dev/null 2>&1 && pm_platform_helper "$LOADER" || true

"$LOADER" --run --root "$GAME_DIR" "$GAME_IMAGE"
RC=$?

echo "Fallback exit code: $RC"
exit "$RC"
