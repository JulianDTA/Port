#ifndef RENDERER_DS_H
#define RENDERER_DS_H

#include <stdint.h>
#include <stdbool.h>
#include "core/card.h"
#include "core/game_state.h"

#define DS_SCREEN_W 256
#define DS_SCREEN_H 192

#define BALATRO_CARD_W 28
#define BALATRO_CARD_H 40

// 15-bit BGR color macro with alpha bit set for DS VRAM bitmaps
#define COLOR_RGB(r, g, b) ((((r) & 0x1F)) | (((g) & 0x1F) << 5) | (((b) & 0x1F) << 10) | 0x8000)

// Balatro Palette Constants (5-bit RGB: 0-31)
#define COLOR_TABLE_BG       COLOR_RGB(4, 9, 7)       // Deep casino green felt
#define COLOR_TABLE_ACCENT   COLOR_RGB(6, 13, 10)     // Table border / felt highlight
#define COLOR_CARD_WHITE     COLOR_RGB(31, 31, 30)    // Crisp card face
#define COLOR_CARD_SHADOW    COLOR_RGB(2, 5, 3)       // Soft drop shadow
#define COLOR_CARD_SELECTED  COLOR_RGB(31, 28, 5)     // Gold glow for selected cards
#define COLOR_HEART_RED      COLOR_RGB(28, 4, 6)      // Balatro Heart crimson
#define COLOR_DIAMOND_ORANGE COLOR_RGB(30, 13, 3)     // Balatro Diamond orange-red
#define COLOR_SPADE_BLACK    COLOR_RGB(4, 5, 8)       // Balatro Spade deep indigo
#define COLOR_CLUB_BLACK     COLOR_RGB(3, 12, 10)     // Balatro Club slate teal

#define COLOR_CHIPS_BLUE     COLOR_RGB(3, 14, 28)     // Balatro Chips badge blue
#define COLOR_MULT_RED       COLOR_RGB(28, 5, 6)      // Balatro Mult badge red
#define COLOR_GOLD_COIN      COLOR_RGB(31, 25, 4)     // Money badge gold
#define COLOR_TEXT_WHITE     COLOR_RGB(31, 31, 31)    // Pure white text
#define COLOR_TEXT_BLACK     COLOR_RGB(2, 2, 2)       // Dark text
#define COLOR_BOX_DARK       COLOR_RGB(2, 4, 3)       // Dark UI panel background
#define COLOR_BOX_BORDER     COLOR_RGB(8, 16, 12)     // UI panel border
#define COLOR_DISCARD_BTN    COLOR_RGB(26, 4, 6)      // Discard button red
#define COLOR_PLAY_BTN       COLOR_RGB(4, 16, 28)     // Play button blue

void renderer_init(void);
void renderer_draw_top_screen(uint16_t* fb, const GameSession* game);
void renderer_draw_bottom_screen(uint16_t* fb, const GameSession* game);

// Drawing primitives
void draw_fill_rect(uint16_t* fb, int x, int y, int w, int h, uint16_t color);
void draw_outline_rect(uint16_t* fb, int x, int y, int w, int h, uint16_t color);
void draw_rounded_panel(uint16_t* fb, int x, int y, int w, int h, uint16_t fill_col, uint16_t border_col);
void draw_string(uint16_t* fb, int x, int y, const char* str, uint16_t text_col);
void draw_string_shadow(uint16_t* fb, int x, int y, const char* str, uint16_t text_col, uint16_t shadow_col);
void draw_string_wrapped(uint16_t* fb, int x, int y, int max_w, const char* str, uint16_t text_col);
void draw_string_clipped(uint16_t* fb, int x, int y, int max_w, const char* str, uint16_t text_col);
void draw_string_2x(uint16_t* fb, int x, int y, const char* str, uint16_t text_col);
void draw_string_2x_shadow(uint16_t* fb, int x, int y, const char* str, uint16_t text_col, uint16_t shadow_col);

#endif // RENDERER_DS_H
