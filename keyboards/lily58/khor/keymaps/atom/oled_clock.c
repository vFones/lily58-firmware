#include "oled_clock.h"

// 7-segment layout for each digit (13 wide x 36 tall, rotated portrait coords)
//
//   x: 0..12  (fits two digits in 32px: digit1 at x=1, digit2 at x=17)
//   y: offset passed by caller
//
//    ##########
//    ##########   <- seg A (top bar):    x+1..x+11, y+0..y+1
//   #          #
//   #          #  <- seg F (top-left):  x+0..x+1,  y+2..y+16
//   #          #  <- seg B (top-right): x+11..x+12, y+2..y+16
//    ##########
//    ##########   <- seg G (middle):    x+1..x+11, y+17..y+18
//   #          #
//   #          #  <- seg E (bot-left):  x+0..x+1,  y+19..y+33
//   #          #  <- seg C (bot-right): x+11..x+12, y+19..y+33
//    ##########
//    ##########   <- seg D (bottom):    x+1..x+11, y+34..y+35

#define SEG_A (1 << 6)
#define SEG_B (1 << 5)
#define SEG_C (1 << 4)
#define SEG_D (1 << 3)
#define SEG_E (1 << 2)
#define SEG_F (1 << 1)
#define SEG_G (1 << 0)

static const uint8_t digit_segs[10] = {
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,           // 0
    SEG_B | SEG_C,                                            // 1
    SEG_A | SEG_B | SEG_D | SEG_E | SEG_G,                   // 2
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,                   // 3
    SEG_B | SEG_C | SEG_F | SEG_G,                           // 4
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,                   // 5
    SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,           // 6
    SEG_A | SEG_B | SEG_C,                                   // 7
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,  // 8
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,           // 9
};

static void draw_hbar(uint8_t x, uint8_t y, uint8_t len) {
    for (uint8_t dy = 0; dy < 2; dy++) {
        for (uint8_t dx = 0; dx < len; dx++) {
            oled_write_pixel(x + dx, y + dy, true);
        }
    }
}

static void draw_vbar(uint8_t x, uint8_t y, uint8_t len) {
    for (uint8_t dy = 0; dy < len; dy++) {
        for (uint8_t dx = 0; dx < 2; dx++) {
            oled_write_pixel(x + dx, y + dy, true);
        }
    }
}

static void draw_digit(uint8_t x, uint8_t y, uint8_t d) {
    uint8_t segs = digit_segs[d % 10];
    if (segs & SEG_A) draw_hbar(x + 1, y,      11);  // top
    if (segs & SEG_F) draw_vbar(x,     y + 2,  15);  // top-left
    if (segs & SEG_B) draw_vbar(x + 11, y + 2, 15);  // top-right
    if (segs & SEG_G) draw_hbar(x + 1, y + 17, 11);  // middle
    if (segs & SEG_E) draw_vbar(x,     y + 19, 15);  // bot-left
    if (segs & SEG_C) draw_vbar(x + 11, y + 19, 15); // bot-right
    if (segs & SEG_D) draw_hbar(x + 1, y + 34, 11);  // bottom
}

// Display layout (32 wide x 128 tall portrait):
//   y=  0..9  : top padding
//   y= 10..45 : HH digits (36px tall)
//   y= 46..52 : gap
//   y= 53..56 : colon dot 1
//   y= 57..61 : gap between dots
//   y= 62..65 : colon dot 2
//   y= 66..72 : gap
//   y= 73..108: MM digits (36px tall)
//   y=109..127: bottom padding
//
// Digit x positions (each digit is 13px wide):
//   digit tens : x=1
//   digit units: x=17
//   colon dots : x=14..17 (4px wide)

void render_clock(uint8_t h, uint8_t m, bool colon_on) {
    oled_scroll_off();
    oled_clear();

    draw_digit(1,  10, h / 10);
    draw_digit(17, 10, h % 10);

    // colon dots — blink on alternate seconds
    if (colon_on) {
        for (uint8_t dy = 0; dy < 4; dy++) {
            for (uint8_t dx = 0; dx < 4; dx++) {
                oled_write_pixel(14 + dx, 53 + dy, true);
                oled_write_pixel(14 + dx, 62 + dy, true);
            }
        }
    }

    draw_digit(1,  73, m / 10);
    draw_digit(17, 73, m % 10);
}
