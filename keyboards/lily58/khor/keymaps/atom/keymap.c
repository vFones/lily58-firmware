#include QMK_KEYBOARD_H
#include <stdio.h>
#include <transactions.h>
#include "oled_glitch.h"
#include "icons.h"
#include "oled_sync.h"

#define RAISE MO(_RAISE)
#define RAISETWO MO(_RAISETWO)
#define BOOT_CONFIRM_TIME 3000
#define USER_ANIM_SYNC_ID ANIM_SYNC

static bool is_windows = false;
static bool boot_pending = false;
static uint16_t boot_timer = 0;
static uint8_t boot_seconds_left = 3;
static uint16_t boot_second_timer = 0;
static uint8_t oled_last_layer = 0xFF;

enum layer_number {
  _QWERTY = 0,
  _RAISE,
  _RAISETWO
};

enum custom_keycodes {
    LRAISE = SAFE_RANGE,
    TG_WIN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |   =  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |   -  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LCTRL |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |   '  |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RCTRL |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |LRAIS | LGUI | LAlt | /Space  /       \Enter \  |BkSp  | RGUI |RAISE |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
    [_QWERTY] = LAYOUT(
      KC_ESC,      KC_1,   KC_2,   KC_3,   KC_4,   KC_5,                               KC_6,   KC_7,   KC_8,       KC_9,   KC_0,       KC_EQUAL,
      KC_TAB,       KC_Q,   KC_W,   KC_E,   KC_R,   KC_T,                               KC_Y,   KC_U,   KC_I,       KC_O,   KC_P,       KC_MINS,
      KC_LCTL,      KC_A,   KC_S,   KC_D,   KC_F,   KC_G,                               KC_H,   KC_J,   KC_K,       KC_L,   KC_SCLN,    KC_QUOT,
      KC_LSFT,      KC_Z,   KC_X,   KC_C,   KC_V,   KC_B,   KC_LBRC,        KC_RBRC,    KC_N,   KC_M,   KC_COMM,    KC_DOT, KC_SLSH,    KC_RCTL,
                                LRAISE, KC_LGUI, KC_LALT,    KC_SPC,         KC_ENT,      KC_BSPC, KC_RGUI, RAISE
    ),
/* RAISE
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | NUBS |  F1  |  F2  |  F3  |  F4  |  F5  |                    |  F6  |  F7  |  F8  |  F9  | F10  | F11  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |                    |      |      | Prev | Next | Play | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |TgWin |      |      |      |      |      |-------.    ,-------| Left | Down |  Up  |Right |      | F13  |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      | Prev | Next | Play | Mute |      |-------|    |-------| End  | PgDn | PgUp | Home |  \   | F14  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |TgWin |      | /       /       \      \  | Del  |RAISE2|      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
    [_RAISE] = LAYOUT(
      KC_GRV,   KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,                                      KC_F6,      KC_F7,      KC_F8,      KC_F9,   KC_F10,    KC_F11,
      _______,  _______,    _______,    _______,    _______,    _______,                                    _______,    _______,    KC_MPRV,    KC_MNXT, KC_MPLY,   KC_F12,
      TG_WIN,   _______,    _______,    _______,    _______,    _______,                                    KC_LEFT,    KC_DOWN,    KC_UP,      KC_RGHT, _______,   KC_F13,
      _______,  KC_MPRV,    KC_MNXT,    KC_MPLY,    KC_MUTE,    _______, _______,               _______,    KC_END,     KC_PGDN,    KC_PGUP,    KC_HOME, KC_BACKSLASH, KC_F14,
                                    _______,  TG_WIN,   _______,         _______,               _______,        KC_DELETE,  RAISETWO,   _______
    ),
/* RAISETWO
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | BOOT |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |REBOOT|      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------.    ,-------|      |      |      |      |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /       /       \      \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
    [_RAISETWO] = LAYOUT(
      QK_BOOT,      _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
      QK_REBOOT,    _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
      _______,      _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
      _______,      _______, _______, _______, _______, _______,    _______,               _______,    _______, _______, _______, _______, _______, _______,
                                _______, _______, _______,          _______,               _______,         _______, _______, _______
    )
};

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  return OLED_ROTATION_270;
}

static const oled_key_event_binding_t oled_key_event_bindings[] = {
    {KC_MUTE, OLED_EVENT_MUTE, render_mute_animation},
    {KC_MPLY, OLED_EVENT_PLAY_PAUSE, render_pause_animation},
    {KC_MNXT, OLED_EVENT_NEXT, render_forward_animation},
    {KC_MPRV, OLED_EVENT_PREV, render_back_animation},
    {RAISE, OLED_EVENT_RAISE, render_raise_icon},
    {LRAISE, OLED_EVENT_LRAISE, render_raise_icon},
    {RAISETWO, OLED_EVENT_RAISE_TWO, render_raise2_icon},
    {KC_UP, OLED_EVENT_UP, render_arrow_up_icon},
    {KC_DOWN, OLED_EVENT_DOWN, render_arrow_down_icon},
    {KC_LEFT, OLED_EVENT_LEFT, render_arrow_left_icon},
    {KC_RGHT, OLED_EVENT_RIGHT, render_arrow_right_icon},
    {KC_HOME, OLED_EVENT_HOME, render_home_icon},
    {KC_END, OLED_EVENT_END, render_end_icon},
    {KC_PGUP, OLED_EVENT_PAGE_UP, render_page_up_icon},
    {KC_PGDN, OLED_EVENT_PAGE_DOWN, render_page_down_icon},
    {KC_F13, OLED_EVENT_F13, render_f13_icon},
    {KC_F14, OLED_EVENT_F14, render_f14_icon}
};

void keyboard_post_init_user(void) {
    oled_sync_init(USER_ANIM_SYNC_ID, oled_key_event_bindings, ARRAY_SIZE(oled_key_event_bindings));
}

static const char PROGMEM big_digits[4][5][5] = {
    {
        "    ",
        "    ",
        "    ",
        "    ",
        "    "
    },
    {
        "  ##",
        "   #",
        "   #",
        "   #",
        "   #"
    },
    {
        "####",
        "   #",
        "####",
        "#   ",
        "####"
    },
    {
        "####",
        "   #",
        "  ##",
        "   #",
        "####"
    }
};

void oled_render_boot_pending(uint8_t seconds) {
    oled_scroll_off();
    oled_clear();

    if (seconds <= 0) {
        oled_write_ln_P(PSTR(""), false);
        oled_write_ln_P(PSTR(""), false);
        oled_write_ln_P(PSTR(""), false);
        oled_write_ln_P(PSTR(""), false);
        oled_write_ln_P(PSTR("BOOTL"), false);
        oled_render_dirty(true);
        return;
    }

    oled_write_ln_P(PSTR("BOOTL"), false);
    oled_write_ln_P(PSTR("LOADS in"), false);
    oled_write_ln_P(PSTR(""), false);

    for (uint8_t row = 0; row < 5; row++) {
        oled_write_P(big_digits[seconds][row], false);
        oled_write_ln_P(PSTR(""), false);
    }

    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("Press"), false);
    oled_write_P(PSTR("a key"), false);
    oled_write_ln_P(PSTR("to"), false);
    oled_write_ln_P(PSTR("abort"), false);
    oled_render_dirty(true);
}

bool oled_task_user(void) {
    if (boot_pending) {
        return false;
    }

    run_pending_oled_events();

    if (oled_swap_anim_task()) {
        return false;
    }
    uint8_t current_layer = get_highest_layer(layer_state);

    if (current_layer != oled_last_layer) {
        oled_clear();
        oled_last_layer = current_layer;
    }

    render_atom_logo_full();

    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    const oled_key_event_binding_t *oled_binding = find_oled_key_event_binding(keycode);

    if (boot_pending && record->event.pressed) {
        boot_pending = false;
        oled_clear();
        return true;
    }

    if (oled_binding) {
        if (record->event.pressed) {
            if (keycode == LRAISE) layer_on(_RAISE);
            bool right_side = record->event.key.row >= MATRIX_ROWS / 2;
            oled_sync_dispatch_press(oled_binding->event, right_side);
        } else {
            if (keycode == LRAISE) layer_off(_RAISE);
            oled_sync_dispatch_release(oled_binding->event);
        }
        return true;
    }

    switch (keycode) {
        case KC_LGUI:
            if (is_windows) {
                if (record->event.pressed) {
                    register_code(KC_LCTL);
                } else {
                    unregister_code(KC_LCTL);
                }
                return false;
            }
            return true;
        case TG_WIN:
            if (record->event.pressed) {
                is_windows = !is_windows;
            }
            return false;
        case QK_BOOT:
            if (record->event.pressed) {
                boot_pending = true;
                boot_seconds_left = 3;
                boot_timer = timer_read();
                boot_second_timer = timer_read();
                oled_render_boot_pending(boot_seconds_left);
            }
            return false;
        default:
            return true;
    }
}

void matrix_scan_user(void) {
    if (!boot_pending) return;

    if (timer_elapsed(boot_second_timer) >= 1000) {
        boot_second_timer = timer_read();

        if (boot_seconds_left > 0) {
            boot_seconds_left--;
            oled_render_boot_pending(boot_seconds_left);
        }
    }

    if (timer_elapsed(boot_timer) >= BOOT_CONFIRM_TIME) {
        boot_pending = false;
        reset_keyboard();
    }
}
