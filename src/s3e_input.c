#include "s3e_host_internal.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

enum {
    SDL_INIT_JOYSTICK = 0x00000200u,
    SDL_INIT_GAMECONTROLLER = 0x00002000u,
};

enum {
    SDL_BUTTON_A = 0,
    SDL_BUTTON_B = 1,
    SDL_BUTTON_X = 2,
    SDL_BUTTON_Y = 3,
    SDL_BUTTON_BACK = 4,
    SDL_BUTTON_START = 6,
    SDL_BUTTON_LEFTSTICK = 7,
    SDL_BUTTON_RIGHTSTICK = 8,
    SDL_BUTTON_LEFTSHOULDER = 9,
    SDL_BUTTON_RIGHTSHOULDER = 10,
    SDL_BUTTON_DPAD_UP = 11,
    SDL_BUTTON_DPAD_DOWN = 12,
    SDL_BUTTON_DPAD_LEFT = 13,
    SDL_BUTTON_DPAD_RIGHT = 14,
};

enum {
    SDL_AXIS_LEFTX = 0,
    SDL_AXIS_LEFTY = 1,
    SDL_AXIS_RIGHTX = 2,
    SDL_AXIS_RIGHTY = 3,
    SDL_AXIS_TRIGGERLEFT = 4,
    SDL_AXIS_TRIGGERRIGHT = 5,
};

enum {
    SDL_HAT_UP = 0x01,
    SDL_HAT_RIGHT = 0x02,
    SDL_HAT_DOWN = 0x04,
    SDL_HAT_LEFT = 0x08,
};

enum {
    POINTER_STATE_UP = 0,
    POINTER_STATE_DOWN = 1,
    POINTER_STATE_PRESSED = 2,
    POINTER_STATE_RELEASED = 4,
};

enum {
    KEY_STATE_DOWN = 1,
    KEY_STATE_PRESSED = 2,
    KEY_STATE_RELEASED = 4,
};

/* NFS Shift (NextOS): teclas s3e que o proprio jogo escuta, direto do ICF
   embutido do modulo ([game] KeyControl* — heranca do Xperia Play):
     MenuUp=10 MenuDown=12 MenuLeft/SteerLeft=9 MenuRight/SteerRight=11
     MenuSelect=8(Space) GearUp=39(Q) GearDown=23(A) Brake=48(Z)
     Drift=46(X) Nitro=41(S) SwitchCamera=44(V) Pause=38(P)
   Aceleracao e' automatica (padrao mobile); nao ha tecla de acelerar. */
enum {
    SHIFT_KEY_ESC = 1,
    SHIFT_KEY_MENU_SELECT = 8,
    SHIFT_KEY_LEFT = 9,
    SHIFT_KEY_UP = 10,
    SHIFT_KEY_RIGHT = 11,
    SHIFT_KEY_DOWN = 12,
    SHIFT_KEY_GEAR_DOWN = 23,
    SHIFT_KEY_PAUSE = 38,
    SHIFT_KEY_GEAR_UP = 39,
    SHIFT_KEY_NITRO = 41,
    SHIFT_KEY_CAMERA = 44,
    SHIFT_KEY_DRIFT = 46,
    SHIFT_KEY_BRAKE = 48,
    S3E_KEY_ABS_GAME_A = 200,
    S3E_KEY_ABS_GAME_B = 201,
    S3E_KEY_ABS_GAME_C = 202,
    S3E_KEY_ABS_GAME_D = 203,
    S3E_KEY_ABS_UP = 204,
    S3E_KEY_ABS_DOWN = 205,
    S3E_KEY_ABS_LEFT = 206,
    S3E_KEY_ABS_RIGHT = 207,
    S3E_KEY_ABS_OK = 208,
    S3E_KEY_ABS_ASK = 209,
    S3E_KEY_ABS_BSK = 210,
};

enum {
    KEYBOARD_KEY_COUNT = 256,
    TOUCHPAD_COUNT = 2,
    AXIS_DEADZONE = 9000,
    XPERIA_AXIS_DEADZONE = 6000,
    TRIGGER_THRESHOLD = 16384,
};

enum {
    S3E_TOUCHPAD_RELEASED = 0,
    S3E_TOUCHPAD_PRESSED = 1,
};

struct key_map {
    const uint32_t *keys;
    size_t key_count;
};

struct sdl_input_api {
    int (*InitSubSystem)(uint32_t flags);
    void (*QuitSubSystem)(uint32_t flags);
    int (*GameControllerAddMapping)(const char *mapping);
    int (*NumJoysticks)(void);
    int (*IsGameController)(int joystick_index);
    void *(*GameControllerOpen)(int joystick_index);
    void (*GameControllerClose)(void *gamecontroller);
    void *(*GameControllerGetJoystick)(void *gamecontroller);
    int (*JoystickNumHats)(void *joystick);
    uint8_t (*JoystickGetHat)(void *joystick, int hat);
    void (*GameControllerUpdate)(void);
    int16_t (*GameControllerGetAxis)(void *gamecontroller, int axis);
    uint8_t (*GameControllerGetButton)(void *gamecontroller, int button);
    const char *(*GetError)(void);
};

static const uint32_t KEY_SELECT[] = {SHIFT_KEY_MENU_SELECT};
static const uint32_t KEY_BACK[] = {SHIFT_KEY_ESC};
static const uint32_t KEY_DRIFT[] = {SHIFT_KEY_DRIFT};
static const uint32_t KEY_CAMERA[] = {SHIFT_KEY_CAMERA};
static const uint32_t KEY_GEAR_DOWN[] = {SHIFT_KEY_GEAR_DOWN};
static const uint32_t KEY_GEAR_UP[] = {SHIFT_KEY_GEAR_UP};
static const uint32_t KEY_BRAKE[] = {SHIFT_KEY_BRAKE};
static const uint32_t KEY_NITRO[] = {SHIFT_KEY_NITRO};
static const uint32_t KEY_PAUSE[] = {SHIFT_KEY_PAUSE};
static const uint32_t KEY_ARROW_UP[] = {SHIFT_KEY_UP};
static const uint32_t KEY_ARROW_DOWN[] = {SHIFT_KEY_DOWN};
static const uint32_t KEY_ARROW_LEFT[] = {SHIFT_KEY_LEFT};
static const uint32_t KEY_ARROW_RIGHT[] = {SHIFT_KEY_RIGHT};

#define KEY_MAP(keys) {keys, ARRAY_SIZE(keys)}

static const struct key_map KEYMAP_SELECT = KEY_MAP(KEY_SELECT);
static const struct key_map KEYMAP_BACK = KEY_MAP(KEY_BACK);
static const struct key_map KEYMAP_DRIFT = KEY_MAP(KEY_DRIFT);
static const struct key_map KEYMAP_CAMERA = KEY_MAP(KEY_CAMERA);
static const struct key_map KEYMAP_GEAR_DOWN = KEY_MAP(KEY_GEAR_DOWN);
static const struct key_map KEYMAP_GEAR_UP = KEY_MAP(KEY_GEAR_UP);
static const struct key_map KEYMAP_BRAKE = KEY_MAP(KEY_BRAKE);
static const struct key_map KEYMAP_NITRO = KEY_MAP(KEY_NITRO);
static const struct key_map KEYMAP_PAUSE = KEY_MAP(KEY_PAUSE);
static const struct key_map KEYMAP_ARROW_UP = KEY_MAP(KEY_ARROW_UP);
static const struct key_map KEYMAP_ARROW_DOWN = KEY_MAP(KEY_ARROW_DOWN);
static const struct key_map KEYMAP_ARROW_LEFT = KEY_MAP(KEY_ARROW_LEFT);
static const struct key_map KEYMAP_ARROW_RIGHT = KEY_MAP(KEY_ARROW_RIGHT);

#undef KEY_MAP

static void *g_sdl2;
static struct sdl_input_api g_sdl;
static void *g_controller;
static void *g_joystick;
static int g_sdl_tried;
static int g_input_pumping;
static int g_keyboard_update_active;
static int g_prev_select;
static int g_prev_a;
static uint64_t g_input_last_ms;
static uint8_t g_hat_mask;

static uint8_t g_keyboard_state[KEYBOARD_KEY_COUNT];

static int g_touchpad_active[TOUCHPAD_COUNT];
static int32_t g_touchpad_x[TOUCHPAD_COUNT];
static int32_t g_touchpad_y[TOUCHPAD_COUNT];
static uint8_t g_touchpad_state[TOUCHPAD_COUNT];

static int sdl_load_symbol(void **slot, const char *name) {
    *slot = dlsym(g_sdl2, name);
    return *slot != NULL;
}

static void sdl_load_optional_symbol(void **slot, const char *name) {
    *slot = dlsym(g_sdl2, name);
}

static void input_open(void) {
    if (g_sdl_tried) {
        return;
    }
    g_sdl_tried = 1;

    const char *names[] = {"libSDL2-2.0.so.0", "libSDL2.so", NULL};
    g_sdl2 = open_first(names);
    if (!g_sdl2) {
        return;
    }

    int ok = 1;
    ok &= sdl_load_symbol((void **)&g_sdl.InitSubSystem, "SDL_InitSubSystem");
    ok &= sdl_load_symbol((void **)&g_sdl.QuitSubSystem, "SDL_QuitSubSystem");
    ok &= sdl_load_symbol((void **)&g_sdl.GameControllerAddMapping, "SDL_GameControllerAddMapping");
    ok &= sdl_load_symbol((void **)&g_sdl.NumJoysticks, "SDL_NumJoysticks");
    ok &= sdl_load_symbol((void **)&g_sdl.IsGameController, "SDL_IsGameController");
    ok &= sdl_load_symbol((void **)&g_sdl.GameControllerOpen, "SDL_GameControllerOpen");
    ok &= sdl_load_symbol((void **)&g_sdl.GameControllerClose, "SDL_GameControllerClose");
    sdl_load_optional_symbol((void **)&g_sdl.GameControllerGetJoystick,
                             "SDL_GameControllerGetJoystick");
    sdl_load_optional_symbol((void **)&g_sdl.JoystickNumHats, "SDL_JoystickNumHats");
    sdl_load_optional_symbol((void **)&g_sdl.JoystickGetHat, "SDL_JoystickGetHat");
    ok &= sdl_load_symbol((void **)&g_sdl.GameControllerUpdate, "SDL_GameControllerUpdate");
    ok &= sdl_load_symbol((void **)&g_sdl.GameControllerGetAxis, "SDL_GameControllerGetAxis");
    ok &= sdl_load_symbol((void **)&g_sdl.GameControllerGetButton, "SDL_GameControllerGetButton");
    sdl_load_optional_symbol((void **)&g_sdl.GetError, "SDL_GetError");
    if (!ok || g_sdl.InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0) {
        if (!ok) {
            fprintf(stderr, "[input] SDL2 controller symbols unavailable\n");
        } else if (g_sdl.GetError) {
            const char *error = g_sdl.GetError();
            fprintf(stderr, "[input] SDL_InitSubSystem failed: %s\n",
                    error ? error : "unknown");
        }
        return;
    }

    const char *mapping = getenv("SDL_GAMECONTROLLERCONFIG");
    if (mapping && mapping[0]) {
        g_sdl.GameControllerAddMapping(mapping);
    }

    int count = g_sdl.NumJoysticks();
    int selected_index = -1;

    for (int i = 0; i < count; ++i) {
        if (g_sdl.IsGameController(i)) {
            selected_index = i;
            break;
        }
    }

    if (selected_index >= 0) {
        g_controller = g_sdl.GameControllerOpen(selected_index);
        if (g_controller) {
            g_joystick =
                g_sdl.GameControllerGetJoystick ? g_sdl.GameControllerGetJoystick(g_controller) :
                                                  NULL;
        }
    }
}

static uint8_t input_current_hat_mask(void) {
    uint8_t mask = 0;
    if (!g_joystick || !g_sdl.JoystickNumHats || !g_sdl.JoystickGetHat) {
        return 0;
    }

    int count = g_sdl.JoystickNumHats(g_joystick);
    for (int i = 0; i < count; ++i) {
        mask |= g_sdl.JoystickGetHat(g_joystick, i);
    }
    return mask;
}

static int input_button(int button) {
    if (!g_controller || !g_sdl.GameControllerGetButton || button < 0) {
        return 0;
    }
    return g_sdl.GameControllerGetButton(g_controller, button) != 0;
}

static int32_t input_axis_raw(int axis) {
    if (!g_controller || !g_sdl.GameControllerGetAxis) {
        return 0;
    }
    return g_sdl.GameControllerGetAxis(g_controller, axis);
}

static int input_trigger(int axis) {
    return input_axis_raw(axis) > TRIGGER_THRESHOLD;
}

static int input_hat(uint8_t mask) {
    return (g_hat_mask & mask) != 0;
}

static int input_dpad_up(void) {
    return input_button(SDL_BUTTON_DPAD_UP) || input_hat(SDL_HAT_UP);
}

static int input_dpad_down(void) {
    return input_button(SDL_BUTTON_DPAD_DOWN) || input_hat(SDL_HAT_DOWN);
}

static int input_dpad_left(void) {
    return input_button(SDL_BUTTON_DPAD_LEFT) || input_hat(SDL_HAT_LEFT);
}

static int input_dpad_right(void) {
    return input_button(SDL_BUTTON_DPAD_RIGHT) || input_hat(SDL_HAT_RIGHT);
}

static int32_t input_axis_deadzone(int axis, int32_t deadzone) {
    int32_t value = input_axis_raw(axis);
    return value < -deadzone || value > deadzone ? value : 0;
}

static int32_t input_axis(int axis) {
    return input_axis_deadzone(axis, AXIS_DEADZONE);
}

static int32_t window_width(void) {
    return g_native_window.width > 0 ? (int32_t)g_native_window.width : 640;
}

static int32_t window_height(void) {
    return g_native_window.height > 0 ? (int32_t)g_native_window.height : 480;
}

static int32_t clamp_value(int32_t value, int32_t upper_exclusive) {
    if (value < 0) {
        return 0;
    }
    if (value >= upper_exclusive) {
        return upper_exclusive - 1;
    }
    return value;
}

static int32_t clamp_pointer_x(int32_t x) {
    return clamp_value(x, window_width());
}

static int32_t clamp_pointer_y(int32_t y) {
    return clamp_value(y, window_height());
}

static void pointer_dispatch(uint32_t id, void *event) {
    if (id >= ARRAY_SIZE(g_pointer_callbacks)) {
        return;
    }
    struct callback_slot *slot = &g_pointer_callbacks[id];
    if (!slot->callback) {
        return;
    }
    ((s3e_callback_fn)(uintptr_t)slot->callback)(event, slot->user_data);
}

static void touchpad_dispatch(uint32_t id, void *event) {
    if (id >= ARRAY_SIZE(g_touchpad_callbacks)) {
        return;
    }
    struct callback_slot *slot = &g_touchpad_callbacks[id];
    if (!slot->callback) {
        return;
    }
    ((s3e_callback_fn)(uintptr_t)slot->callback)(event, slot->user_data);
}

static void pointer_set_down(int down) {
    if (down) {
        g_pointer_states[0] = g_pointer_down ? POINTER_STATE_DOWN : POINTER_STATE_PRESSED;
        g_pointer_down = 1;
    } else {
        g_pointer_states[0] = g_pointer_down ? POINTER_STATE_RELEASED : POINTER_STATE_UP;
        g_pointer_down = 0;
    }
}

static void pointer_clear_transitions(void) {
    for (size_t i = 0; i < sizeof(g_pointer_states); ++i) {
        if (g_pointer_states[i] == POINTER_STATE_PRESSED) {
            g_pointer_states[i] = POINTER_STATE_DOWN;
        } else if (g_pointer_states[i] == POINTER_STATE_RELEASED) {
            g_pointer_states[i] = POINTER_STATE_UP;
        }
    }
    for (size_t i = 0; i < ARRAY_SIZE(g_touchpad_state); ++i) {
        if (g_touchpad_state[i] == POINTER_STATE_PRESSED) {
            g_touchpad_state[i] = POINTER_STATE_DOWN;
        } else if (g_touchpad_state[i] == POINTER_STATE_RELEASED) {
            g_touchpad_state[i] = POINTER_STATE_UP;
        }
    }
}

static void pointer_dispatch_button(uint32_t button, int32_t pressed) {
    struct s3e_pointer_button_event event = {
        .button = (int32_t)button,
        .pressed = pressed,
        .x = g_pointer_x,
        .y = g_pointer_y,
    };
    struct s3e_pointer_touch_event touch_event = {
        .touch_id = (int32_t)button,
        .pressed = pressed,
        .x = g_pointer_x,
        .y = g_pointer_y,
    };
    pointer_dispatch(0, &event);
    pointer_dispatch(2, &touch_event);
}

static void pointer_dispatch_motion(void) {
    struct s3e_pointer_motion_event event = {
        .x = g_pointer_x,
        .y = g_pointer_y,
    };
    struct s3e_pointer_touch_motion_event touch_event = {
        .touch_id = 0,
        .x = g_pointer_x,
        .y = g_pointer_y,
    };
    pointer_dispatch(1, &event);
    pointer_dispatch(3, &touch_event);
}

static void input_release_pointer(void) {
    if (g_pointer_down) {
        pointer_set_down(0);
        pointer_dispatch_button(0, 0);
    }
}

static void keyboard_dispatch_event(uint32_t key, int32_t pressed) {
    struct s3e_keyboard_event event = {
        .key = (int32_t)key,
        .pressed = pressed ? 1 : 0,
    };

    for (size_t i = 0; i < ARRAY_SIZE(g_keyboard_callbacks); ++i) {
        struct keyboard_callback_slot *slot = &g_keyboard_callbacks[i];
        if (slot->callback) {
            ((s3e_callback_fn)(uintptr_t)slot->callback)(&event, slot->user_data);
        }
    }
}

static void keyboard_set_key(uint32_t key, int down, int dispatch_callback) {
    if (key >= KEYBOARD_KEY_COUNT) {
        return;
    }

    int was_down = (g_keyboard_state[key] & KEY_STATE_DOWN) != 0;
    if (was_down == down) {
        return;
    }

    if (down) {
        g_keyboard_state[key] &= (uint8_t)~KEY_STATE_RELEASED;
        g_keyboard_state[key] |= KEY_STATE_DOWN | KEY_STATE_PRESSED;
    } else {
        g_keyboard_state[key] &= (uint8_t)~KEY_STATE_DOWN;
        g_keyboard_state[key] &= (uint8_t)~KEY_STATE_PRESSED;
        g_keyboard_state[key] |= KEY_STATE_RELEASED;
    }
    if (dispatch_callback) {
        keyboard_dispatch_event(key, down);
    }
}

static void keyboard_set_keys(const uint32_t *keys, size_t count, int down, int dispatch_callback) {
    for (size_t i = 0; i < count; ++i) {
        keyboard_set_key(keys[i], down, dispatch_callback);
    }
}

static void keyboard_clear_transitions(void) {
    for (size_t key = 0; key < ARRAY_SIZE(g_keyboard_state); ++key) {
        g_keyboard_state[key] &= (uint8_t)~(KEY_STATE_PRESSED | KEY_STATE_RELEASED);
    }
}

static uint32_t keyboard_abs_target(uint32_t key) {
    switch (key) {
    case S3E_KEY_ABS_GAME_A:
        return SHIFT_KEY_MENU_SELECT;
    case S3E_KEY_ABS_GAME_B:
        return SHIFT_KEY_ESC;
    case S3E_KEY_ABS_GAME_C:
        return SHIFT_KEY_DRIFT;
    case S3E_KEY_ABS_GAME_D:
        return SHIFT_KEY_CAMERA;
    case S3E_KEY_ABS_UP:
        return SHIFT_KEY_UP;
    case S3E_KEY_ABS_DOWN:
        return SHIFT_KEY_DOWN;
    case S3E_KEY_ABS_LEFT:
        return SHIFT_KEY_LEFT;
    case S3E_KEY_ABS_RIGHT:
        return SHIFT_KEY_RIGHT;
    case S3E_KEY_ABS_OK:
        return SHIFT_KEY_MENU_SELECT;
    case S3E_KEY_ABS_ASK:
        return SHIFT_KEY_PAUSE;
    case S3E_KEY_ABS_BSK:
        return SHIFT_KEY_ESC;
    default:
        return key;
    }
}

static void game_action_apply(int physical_down, const struct key_map *keys) {
    keyboard_set_keys(keys->keys, keys->key_count, physical_down, 1);
}

static void keyboard_release_all(void) {
    for (uint32_t key = 0; key < KEYBOARD_KEY_COUNT; ++key) {
        if (g_keyboard_state[key] & KEY_STATE_DOWN) {
            keyboard_set_key(key, 0, 1);
        }
    }
}

static void touchpad_dispatch_button(uint32_t id, int32_t pressed, int32_t x, int32_t y) {
    struct s3e_touchpad_button_event event = {
        .id = (int32_t)id,
        .pressed = pressed,
        .x = x,
        .y = y,
    };
    touchpad_dispatch(0, &event);
}

static void __attribute__((unused)) touchpad_dispatch_motion(uint32_t id, int32_t x, int32_t y) {
    struct s3e_touchpad_motion_event event = {
        .id = (int32_t)id,
        .x = x,
        .y = y,
    };
    touchpad_dispatch(1, &event);
}

static void touchpad_release(uint32_t id) {
    if (id >= TOUCHPAD_COUNT || !g_touchpad_active[id]) {
        return;
    }
    g_touchpad_active[id] = 0;
    g_touchpad_state[id] = POINTER_STATE_RELEASED;
    touchpad_dispatch_button(id, S3E_TOUCHPAD_RELEASED, g_touchpad_x[id], g_touchpad_y[id]);
}

static void touchpad_release_all(void) {
    for (uint32_t id = 0; id < TOUCHPAD_COUNT; ++id) {
        touchpad_release(id);
    }
}

static void input_update_cursor(uint64_t dt) {
    int a = input_button(SDL_BUTTON_A);
    if (a != g_prev_a) {
        pointer_set_down(a);
        pointer_dispatch_button(0, a ? 1 : 0);
    }
    g_prev_a = a;

    int32_t x_axis = input_axis(SDL_AXIS_LEFTX);
    int32_t y_axis = input_axis(SDL_AXIS_LEFTY);
    if (input_dpad_left()) {
        x_axis = -32767;
    } else if (input_dpad_right()) {
        x_axis = 32767;
    }
    if (input_dpad_up()) {
        y_axis = -32767;
    } else if (input_dpad_down()) {
        y_axis = 32767;
    }
    if (!x_axis && !y_axis) {
        return;
    }

    int32_t old_x = g_pointer_x;
    int32_t old_y = g_pointer_y;
    const int32_t speed = 900;
    g_pointer_x = clamp_pointer_x(g_pointer_x +
                                  (int32_t)((int64_t)x_axis * (int64_t)dt * speed / 32767 / 1000));
    g_pointer_y = clamp_pointer_y(g_pointer_y +
                                  (int32_t)((int64_t)y_axis * (int64_t)dt * speed / 32767 / 1000));
    if (g_pointer_x != old_x || g_pointer_y != old_y) {
        pointer_dispatch_motion();
    }
}

static void input_update_game_keys(void) {
    g_prev_a = input_button(SDL_BUTTON_A);

    /* NFS Shift (NextOS): mapeamento Xbox-padrao dos nossos ports de corrida
       (igual NFS Hot Pursuit: nitro=R2, freio=L2, START=pausa). Aceleracao e'
       automatica no jogo. Setas = menu + esterco digital (KeySteer* do ICF). */
    game_action_apply(input_button(SDL_BUTTON_A), &KEYMAP_SELECT);
    game_action_apply(input_button(SDL_BUTTON_B), &KEYMAP_BACK);
    game_action_apply(input_button(SDL_BUTTON_X), &KEYMAP_DRIFT);
    game_action_apply(input_button(SDL_BUTTON_Y), &KEYMAP_CAMERA);
    game_action_apply(input_button(SDL_BUTTON_LEFTSHOULDER), &KEYMAP_GEAR_DOWN);
    game_action_apply(input_button(SDL_BUTTON_RIGHTSHOULDER), &KEYMAP_GEAR_UP);
    game_action_apply(input_trigger(SDL_AXIS_TRIGGERLEFT), &KEYMAP_BRAKE);
    game_action_apply(input_trigger(SDL_AXIS_TRIGGERRIGHT), &KEYMAP_NITRO);
    game_action_apply(input_button(SDL_BUTTON_START), &KEYMAP_PAUSE);

    /* Setas: dpad OU stick esquerdo -> s3eKeyLeft/Up/Right/Down (9/10/11/12).
       O proprio jogo le essas teclas p/ navegar menu e estercar (Xperia Play). */
    int32_t lx = input_axis(SDL_AXIS_LEFTX);
    int32_t ly = input_axis(SDL_AXIS_LEFTY);
    game_action_apply(input_dpad_left() || lx < 0, &KEYMAP_ARROW_LEFT);
    game_action_apply(input_dpad_right() || lx > 0, &KEYMAP_ARROW_RIGHT);
    game_action_apply(input_dpad_up() || ly < 0, &KEYMAP_ARROW_UP);
    game_action_apply(input_dpad_down() || ly > 0, &KEYMAP_ARROW_DOWN);
}

static void input_update_game_touchpads(void) {
    /* NFS Shift nao registra os touchpads Xperia (extensao ausente nos imports
       do modulo) — nada a alimentar aqui. */
}

void input_pump(void) {
    if (g_input_pumping) {
        return;
    }
    g_input_pumping = 1;

    input_open();
    if (!g_controller) {
        goto out;
    }
    g_sdl.GameControllerUpdate();
    g_hat_mask = input_current_hat_mask();

    /* NextOS: Select+Start = sair do port (hotkey padrao dos nossos ports).
       O launcher restaura o free_scale ao ver o processo terminar. */
    if (input_button(SDL_BUTTON_BACK) && input_button(SDL_BUTTON_START)) {
        _exit(0);
    }

    uint64_t now = monotonic_ms();
    if (!g_input_last_ms) {
        g_input_last_ms = now;
    }
    uint64_t dt = now - g_input_last_ms;
    g_input_last_ms = now;
    if (dt > 50) {
        dt = 50;
    }

    int select = input_button(SDL_BUTTON_BACK);
    if (select && !g_prev_select) {
        if (g_cursor_active) {
            input_release_pointer();
        } else {
            touchpad_release_all();
            keyboard_release_all();
        }
        g_cursor_active = !g_cursor_active;
    }
    g_prev_select = select;

    if (g_cursor_active) {
        touchpad_release_all();
        if (g_keyboard_update_active) {
            keyboard_release_all();
        }
        input_update_cursor(dt);
    } else {
        input_release_pointer();
        input_update_game_touchpads();
        if (g_keyboard_update_active) {
            input_update_game_keys();
        }
    }

out:
    g_input_pumping = 0;
}

void input_shutdown(void) {
    input_release_pointer();
    touchpad_release_all();
    keyboard_release_all();
    if (g_controller && g_sdl.GameControllerClose) {
        g_sdl.GameControllerClose(g_controller);
        g_controller = NULL;
        g_joystick = NULL;
    }
    if (g_sdl2) {
        if (g_sdl.QuitSubSystem) {
            g_sdl.QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
        }
        dlclose(g_sdl2);
        g_sdl2 = NULL;
    }
}

int32_t s3eKeyboardRegister(uint32_t id, void *callback, void *user_data) {
    struct keyboard_callback_slot *free_slot = NULL;
    for (size_t i = 0; i < ARRAY_SIZE(g_keyboard_callbacks); ++i) {
        struct keyboard_callback_slot *slot = &g_keyboard_callbacks[i];
        if (slot->callback == callback && slot->id == id) {
            slot->user_data = user_data;
            return 0;
        }
        if (!slot->callback && !free_slot) {
            free_slot = slot;
        }
    }
    if (free_slot) {
        free_slot->id = id;
        free_slot->callback = callback;
        free_slot->user_data = user_data;
    }
    return 0;
}

int32_t s3eKeyboardUnRegister(uint32_t id, void *callback) {
    for (size_t i = 0; i < ARRAY_SIZE(g_keyboard_callbacks); ++i) {
        struct keyboard_callback_slot *slot = &g_keyboard_callbacks[i];
        if (slot->callback && slot->id == id && (!callback || callback == slot->callback)) {
            slot->callback = NULL;
            slot->user_data = NULL;
        }
    }
    return 0;
}

int32_t s3eKeyboardUpdate(void) {
    keyboard_clear_transitions();
    g_keyboard_update_active = 1;
    input_pump();
    if (g_cursor_active || !g_controller) {
        keyboard_release_all();
    }
    g_keyboard_update_active = 0;
    dispatch_due_timers();
    return 0;
}

int32_t s3eKeyboardGetState(uint32_t key) {
    uint32_t target = keyboard_abs_target(key);
    return target < KEYBOARD_KEY_COUNT ? g_keyboard_state[target] : 0;
}

int32_t s3eKeyboardAnyKey(void) {
    for (size_t i = 0; i < ARRAY_SIZE(g_keyboard_state); ++i) {
        if (g_keyboard_state[i] & (KEY_STATE_DOWN | KEY_STATE_PRESSED)) {
            return 1;
        }
    }
    return 0;
}

int32_t s3eKeyboardGetInt(uint32_t key) {
    switch (key) {
    case 0:
    case 2:
        return 1;
    case 1:
    case 4:
    case 5:
    case 6:
        return 0;
    default:
        return -1;
    }
}

int32_t s3eKeyboardSetInt(uint32_t key, int32_t value) {
    (void)key;
    (void)value;
    return 0;
}

const char *s3eKeyboardGetDisplayName(uint32_t key) {
    switch (key) {
    case SHIFT_KEY_MENU_SELECT:
        return "Select";
    case SHIFT_KEY_ESC:
        return "Back";
    case SHIFT_KEY_LEFT:
        return "Left";
    case SHIFT_KEY_UP:
        return "Up";
    case SHIFT_KEY_RIGHT:
        return "Right";
    case SHIFT_KEY_DOWN:
        return "Down";
    case SHIFT_KEY_GEAR_DOWN:
        return "GearDown";
    case SHIFT_KEY_GEAR_UP:
        return "GearUp";
    case SHIFT_KEY_BRAKE:
        return "Brake";
    case SHIFT_KEY_NITRO:
        return "Nitro";
    case SHIFT_KEY_DRIFT:
        return "Drift";
    case SHIFT_KEY_CAMERA:
        return "Camera";
    case SHIFT_KEY_PAUSE:
        return "Pause";
    case S3E_KEY_ABS_GAME_A:
        return "KeyAbsGameA";
    case S3E_KEY_ABS_GAME_B:
        return "KeyAbsGameB";
    case S3E_KEY_ABS_GAME_C:
        return "KeyAbsGameC";
    case S3E_KEY_ABS_GAME_D:
        return "KeyAbsGameD";
    case S3E_KEY_ABS_UP:
        return "KeyAbsUp";
    case S3E_KEY_ABS_DOWN:
        return "KeyAbsDown";
    case S3E_KEY_ABS_LEFT:
        return "KeyAbsLeft";
    case S3E_KEY_ABS_RIGHT:
        return "KeyAbsRight";
    case S3E_KEY_ABS_OK:
        return "KeyAbsOk";
    case S3E_KEY_ABS_ASK:
        return "KeyAbsASK";
    case S3E_KEY_ABS_BSK:
        return "KeyAbsBSK";
    default:
        return "";
    }
}

void s3eKeyboardClearState(void) {
    memset(g_keyboard_state, 0, sizeof(g_keyboard_state));
}

int32_t s3ePointerRegister(uint32_t id, void *callback, void *user_data) {
    if (id < ARRAY_SIZE(g_pointer_callbacks)) {
        g_pointer_callbacks[id].callback = callback;
        g_pointer_callbacks[id].user_data = user_data;
    }
    return 0;
}

int32_t s3ePointerUnRegister(uint32_t id, void *callback) {
    if (id < ARRAY_SIZE(g_pointer_callbacks) &&
        (!callback || callback == g_pointer_callbacks[id].callback)) {
        g_pointer_callbacks[id].callback = NULL;
        g_pointer_callbacks[id].user_data = NULL;
    }
    return 0;
}

int32_t s3ePointerUpdate(void) {
    pointer_clear_transitions();
    input_pump();
    dispatch_due_timers();
    return 0;
}

int32_t s3ePointerGetInt(uint32_t key) {
    input_pump();
    switch (key) {
    case 0:
        return 1;
    case 1:
        return g_pointer_x;
    case 2:
        return g_pointer_y;
    default:
        return 0;
    }
}

int32_t s3ePointerSetInt(uint32_t key, int32_t value) {
    (void)key;
    (void)value;
    return 0;
}

int32_t s3ePointerGetState(uint32_t button) {
    input_pump();
    return button < sizeof(g_pointer_states) ? g_pointer_states[button] : 0;
}

int32_t s3ePointerGetX(void) {
    input_pump();
    return g_pointer_x;
}

int32_t s3ePointerGetY(void) {
    input_pump();
    return g_pointer_y;
}

int32_t s3ePointerGetTouchState(uint32_t touch_id) {
    input_pump();
    if (g_cursor_active) {
        return touch_id == 0 ? g_pointer_states[0] : 0;
    }
    return 0;
}

int32_t s3ePointerGetTouchX(uint32_t touch_id) {
    input_pump();
    if (g_cursor_active) {
        return touch_id == 0 ? g_pointer_x : 0;
    }
    return 0;
}

int32_t s3ePointerGetTouchY(uint32_t touch_id) {
    input_pump();
    if (g_cursor_active) {
        return touch_id == 0 ? g_pointer_y : 0;
    }
    return 0;
}

int32_t s3ePointerGetPressure(uint32_t button) {
    input_pump();
    return button == 0 && g_pointer_down ? 1 : 0;
}

int32_t s3ePointerGetTouchPressure(uint32_t touch_id) {
    input_pump();
    if (g_cursor_active) {
        return touch_id == 0 && g_pointer_down ? 1 : 0;
    }
    return 0;
}

int32_t s3ePointerGetError(void) {
    return 0;
}

const char *s3ePointerGetErrorString(void) {
    return "S3E_POINTER_ERR_NONE";
}
