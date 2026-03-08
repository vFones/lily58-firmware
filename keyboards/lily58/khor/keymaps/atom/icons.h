#pragma once
#include QMK_KEYBOARD_H

#define MUTE_ANIM_MS      220   // delay between swaps (slower)
#define MUTE_ANIM_FRAMES  8     // number of swaps total
#define MUTE_ANIM_HOLD_MS 800   // hold final frame before releasing (optional)
#define OLED_FRAME_BYTES (OLED_DISPLAY_WIDTH * (OLED_DISPLAY_HEIGHT / 8))

void oled_anim_release(void);
bool oled_swap_anim_task(void);

void render_mute_animation(void);
void render_pause_animation(void);
void render_forward_animation(void);
void render_back_animation(void);
void render_raise_icon(void);
void render_raise2_icon(void);
void render_arrow_up_icon(void);
void render_arrow_down_icon(void);
void render_arrow_left_icon(void);
void render_arrow_right_icon(void);
void render_home_icon(void);
void render_end_icon(void);
void render_page_up_icon(void);
void render_page_down_icon(void);
void render_f13_icon(void);
void render_f14_icon(void);

#define LOGO_SCROLL_PAUSE_MS     0
#define LOGO_SCROLL_MS_PER_PIXEL 50

void render_atom_logo_full(void);
void pause_logo_scroll(uint16_t pause_ms);
