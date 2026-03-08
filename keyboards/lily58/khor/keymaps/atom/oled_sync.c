#include "oled_sync.h"
#include "user_transport.h"
#include "icons.h"

enum sync_cmd {
    SYNC_CMD_OLED_EVENT = 0,
    SYNC_CMD_OLED_RELEASE,
    SYNC_CMD_PAUSE_SCROLL,
};

typedef struct {
    uint8_t cmd;
    uint8_t data_lo;
    uint8_t data_hi;
} sync_packet_t;

static uint8_t sync_transaction_id = 0;
static bool oled_event_pending[OLED_EVENT_COUNT] = {0};
static const oled_key_event_binding_t *sync_bindings = NULL;
static uint8_t sync_bindings_count = 0;

static void sync_send(uint8_t cmd, uint16_t data) {
    sync_packet_t packet = {
        .cmd     = cmd,
        .data_lo = (uint8_t)(data & 0xFF),
        .data_hi = (uint8_t)((data >> 8) & 0xFF),
    };
    user_transport_send(sync_transaction_id, &packet, sizeof(packet));
}

void run_pending_oled_events(void) {
    for (uint8_t i = 0; i < sync_bindings_count; i++) {
        const oled_key_event_binding_t *binding = &sync_bindings[i];
        if (oled_event_pending[binding->event]) {
            oled_event_pending[binding->event] = false;
            binding->on_press();
        }
    }
}

const oled_key_event_binding_t *find_oled_key_event_binding(uint16_t keycode) {
    for (uint8_t i = 0; i < sync_bindings_count; i++) {
        if (sync_bindings[i].keycode == keycode) {
            return &sync_bindings[i];
        }
    }
    return NULL;
}

// All process_record_user calls run on master. Route to the display on the
// same side the key was physically pressed: queue locally for left-side keys
// (master renders), or send to slave for right-side keys (slave renders).
void oled_sync_dispatch_press(enum oled_event event, bool right_side) {
    if (right_side) {
        sync_send(SYNC_CMD_PAUSE_SCROLL, LOGO_SCROLL_PAUSE_MS);
        sync_send(SYNC_CMD_OLED_EVENT, (uint16_t)event);
    } else {
        pause_logo_scroll(LOGO_SCROLL_PAUSE_MS);
        oled_event_pending[event] = true;
    }
}

// Broadcast release to both sides; oled_anim_release() is a no-op on the
// side that never started an animation.
void oled_sync_dispatch_release(enum oled_event event) {
    oled_anim_release();
    sync_send(SYNC_CMD_OLED_RELEASE, 0);
}

static void handle_transport_data(const void *data, uint8_t size) {
    if (data == NULL || size < sizeof(sync_packet_t)) return;

    const sync_packet_t *packet  = (const sync_packet_t *)data;
    uint16_t             payload = (uint16_t)packet->data_lo | ((uint16_t)packet->data_hi << 8);

    switch (packet->cmd) {
        case SYNC_CMD_OLED_EVENT:
            if (payload < OLED_EVENT_COUNT) {
                oled_event_pending[payload] = true;
            }
            return;
        case SYNC_CMD_OLED_RELEASE:
            oled_anim_release();
            return;
        case SYNC_CMD_PAUSE_SCROLL:
            pause_logo_scroll(payload);
            return;
        default:
            return;
    }
}

void oled_sync_init(uint8_t sync_id, const oled_key_event_binding_t *bindings, uint8_t count) {
    sync_transaction_id = sync_id;
    sync_bindings       = bindings;
    sync_bindings_count = count;
    user_transport_init(sync_id, handle_transport_data);
}
