#pragma once

#include QMK_KEYBOARD_H

typedef void (*user_transport_handler_t)(const void *data, uint8_t size);

void user_transport_init(int8_t transaction_id, user_transport_handler_t handler);
void user_transport_send(int8_t transaction_id, const void *data, uint8_t size);
