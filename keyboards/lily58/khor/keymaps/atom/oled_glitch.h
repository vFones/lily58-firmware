#pragma once
#include QMK_KEYBOARD_H

#define GLITCH_FRAME_MS 20

// Call to start a glitch burst lasting `frames` mutations.
void oled_glitch_start(uint16_t frames);

// Call every OLED frame after you've prepared your raw buffer.
// It mutates `buf` in-place when glitch is active.
void oled_glitch_apply(uint8_t *buf, uint16_t size);
