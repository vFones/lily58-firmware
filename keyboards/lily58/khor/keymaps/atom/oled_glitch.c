#include "oled_glitch.h"
#include <oled_driver.h>

static uint32_t rng_state = 1;
static inline uint32_t xs32(void) {
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}
static inline uint8_t u8rand(void) { return (uint8_t)xs32(); }

static uint16_t glitch_left = 0;
static uint32_t glitch_timer = 0;

#define GLITCH_FRAME_MS 20

void oled_glitch_start(uint16_t frames) {
    rng_state ^= timer_read32() + 0x9E3779B9u;
    glitch_left = frames;
    glitch_timer = timer_read32();
}

void oled_glitch_apply(uint8_t *buf, uint16_t size) {
    if (!glitch_left) return;
    if (timer_elapsed32(glitch_timer) < GLITCH_FRAME_MS) return;

    glitch_timer = timer_read32();
    glitch_left--;

    const uint16_t w = OLED_DISPLAY_WIDTH;
    const uint16_t pages = OLED_DISPLAY_HEIGHT / 8;
    if (!w || !pages || size < (w * pages)) return;

    uint8_t p0 = (uint8_t)(u8rand() % pages);
    uint8_t band = (uint8_t)(1 + (u8rand() % 2));
    int8_t  dx = (int8_t)((u8rand() % 9) - 4);

    for (uint8_t p = p0; p < pages && p < (uint8_t)(p0 + band); p++) {
        uint16_t row = (uint16_t)p * w;

        if (dx > 0) {
            for (int16_t x = (int16_t)w - 1; x >= dx; x--) buf[row + x] = buf[row + (x - dx)];
            for (uint8_t x = 0; x < (uint8_t)dx; x++) buf[row + x] = u8rand();
        } else if (dx < 0) {
            uint8_t s = (uint8_t)(-dx);
            for (uint16_t x = 0; x + s < w; x++) buf[row + x] = buf[row + x + s];
            for (uint16_t x = w - s; x < w; x++) buf[row + x] = u8rand();
        }
    }
    for (uint8_t i = 0; i < 18; i++) {
        uint16_t k = (uint16_t)u8rand() | ((uint16_t)u8rand() << 8);
        k %= size;
        buf[k] ^= (uint8_t)(1u << (u8rand() & 7));
    }
    if ((u8rand() & 3u) == 0) {
        uint8_t p = (uint8_t)(u8rand() % pages);
        uint16_t row = (uint16_t)p * w;
        for (uint16_t x = 0; x < w; x++) buf[row + x] ^= 0xFF;
    }
}
