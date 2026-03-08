#pragma once
#include QMK_KEYBOARD_H

typedef void (*oled_event_handler_t)(void);

enum oled_event {
    OLED_EVENT_MUTE = 0,
    OLED_EVENT_PLAY_PAUSE,
    OLED_EVENT_NEXT,
    OLED_EVENT_PREV,
    OLED_EVENT_RAISE,
    OLED_EVENT_LRAISE,
    OLED_EVENT_RAISE_TWO,
    OLED_EVENT_UP,
    OLED_EVENT_DOWN,
    OLED_EVENT_LEFT,
    OLED_EVENT_RIGHT,
    OLED_EVENT_HOME,
    OLED_EVENT_END,
    OLED_EVENT_PAGE_UP,
    OLED_EVENT_PAGE_DOWN,
    OLED_EVENT_F13,
    OLED_EVENT_F14,
    OLED_EVENT_COUNT
};

typedef struct {
    uint16_t keycode;
    enum oled_event event;
    oled_event_handler_t on_press;
} oled_key_event_binding_t;

void oled_sync_init(uint8_t sync_id, const oled_key_event_binding_t *bindings, uint8_t count);
void run_pending_oled_events(void);
const oled_key_event_binding_t *find_oled_key_event_binding(uint16_t keycode);
void oled_sync_dispatch_press(enum oled_event event, bool right_side);
void oled_sync_dispatch_release(enum oled_event event);
