#include "user_transport.h"
#include <transactions.h>

static user_transport_handler_t g_handler = NULL;

static void user_transport_rpc_handler(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer, uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    (void)target2initiator_buffer_size;
    (void)target2initiator_buffer;

    if (g_handler == NULL || initiator2target_buffer == NULL || initiator2target_buffer_size == 0) {
        return;
    }

    g_handler(initiator2target_buffer, initiator2target_buffer_size);
}

void user_transport_init(int8_t transaction_id, user_transport_handler_t handler) {
    if (transaction_id < 0 || handler == NULL) {
        return;
    }

    g_handler = handler;
    transaction_register_rpc(transaction_id, user_transport_rpc_handler);
}

void user_transport_send(int8_t transaction_id, const void *data, uint8_t size) {
    if (transaction_id < 0) {
        return;
    }

    if (!is_keyboard_master() || data == NULL || size == 0) {
        return;
    }

    transaction_rpc_send(transaction_id, size, data);
}
