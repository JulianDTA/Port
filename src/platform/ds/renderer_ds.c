#include "platform/renderer_ds.h"
#include "gfx/balatro_deck_gfx.h"
#include "gfx/balatro_chips_gfx.h"
#include <stdio.h>
#include <string.h>

// 5x7 ASCII bitmap font (characters 32 ' ' to 126 '~')
static const uint8_t s_font5x7[95][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // 32 ' '
    {0x00,0x00,0x5F,0x00,0x00}, // 33 '!'
    {0x00,0x07,0x00,0x07,0x00}, // 34 '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // 35 '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // 36 '$'
    {0x23,0x13,0x08,0x64,0x62}, // 37 '%'
    {0x36,0x49,0x55,0x22,0x50}, // 38 '&'
    {0x00,0x05,0x03,0x00,0x00}, // 39 '''
    {0x00,0x1C,0x22,0x41,0x00}, // 40 '('
    {0x00,0x41,0x22,0x1C,0x00}, // 41 ')'
    {0x14,0x08,0x3E,0x08,0x14}, // 42 '*'
    {0x08,0x08,0x3E,0x08,0x08}, // 43 '+'
    {0x00,0x50,0x30,0x00,0x00}, // 44 ','
    {0x08,0x08,0x08,0x08,0x08}, // 45 '-'
    {0x00,0x60,0x60,0x00,0x00}, // 46 '.'
    {0x20,0x10,0x08,0x04,0x02}, // 47 '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // 48 '0'
    {0x00,0x42,0x7F,0x40,0x00}, // 49 '1'
    {0x42,0x61,0x51,0x49,0x46}, // 50 '2'
    {0x21,0x41,0x45,0x4B,0x31}, // 51 '3'
    {0x18,0x14,0x12,0x7F,0x10}, // 52 '4'
    {0x27,0x45,0x45,0x45,0x39}, // 53 '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // 54 '6'
    {0x01,0x71,0x09,0x05,0x03}, // 55 '7'
    {0x36,0x49,0x49,0x49,0x36}, // 56 '8'
    {0x06,0x49,0x49,0x29,0x1E}, // 57 '9'
    {0x00,0x36,0x36,0x00,0x00}, // 58 ':'
    {0x00,0x56,0x36,0x00,0x00}, // 59 ';'
    {0x08,0x14,0x22,0x41,0x00}, // 60 '<'
    {0x14,0x14,0x14,0x14,0x14}, // 61 '='
    {0x00,0x41,0x22,0x14,0x08}, // 62 '>'
    {0x02,0x01,0x51,0x09,0x06}, // 63 '?'
    {0x32,0x49,0x79,0x41,0x3E}, // 64 '@'
    {0x7E,0x11,0x11,0x11,0x7E}, // 65 'A'
    {0x7F,0x49,0x49,0x49,0x36}, // 66 'B'
    {0x3E,0x41,0x41,0x41,0x22}, // 67 'C'
    {0x7F,0x41,0x41,0x22,0x1C}, // 68 'D'
    {0x7F,0x49,0x49,0x49,0x41}, // 69 'E'
    {0x7F,0x09,0x09,0x09,0x06}, // 70 'F'
    {0x3E,0x41,0x49,0x49,0x7A}, // 71 'G'
    {0x7F,0x08,0x08,0x08,0x7F}, // 72 'H'
    {0x00,0x41,0x7F,0x41,0x00}, // 73 'I'
    {0x20,0x40,0x41,0x3F,0x01}, // 74 'J'
    {0x7F,0x08,0x14,0x22,0x41}, // 75 'K'
    {0x7F,0x40,0x40,0x40,0x40}, // 76 'L'
    {0x7F,0x02,0x0C,0x02,0x7F}, // 77 'M'
    {0x7F,0x04,0x08,0x10,0x7F}, // 78 'N'
    {0x3E,0x41,0x41,0x41,0x3E}, // 79 'O'
    {0x7F,0x09,0x09,0x09,0x06}, // 80 'P'
    {0x3E,0x41,0x51,0x21,0x5E}, // 81 'Q'
    {0x7F,0x09,0x19,0x29,0x46}, // 82 'R'
    {0x46,0x49,0x49,0x49,0x31}, // 83 'S'
    {0x01,0x01,0x7F,0x01,0x01}, // 84 'T'
    {0x3F,0x40,0x40,0x40,0x3F}, // 85 'U'
    {0x1F,0x20,0x40,0x20,0x1F}, // 86 'V'
    {0x3F,0x40,0x38,0x40,0x3F}, // 87 'W'
    {0x63,0x14,0x08,0x14,0x63}, // 88 'X'
    {0x07,0x08,0x70,0x08,0x07}, // 89 'Y'
    {0x61,0x51,0x49,0x45,0x43}, // 90 'Z'
    {0x00,0x7F,0x41,0x41,0x00}, // 91 '['
    {0x02,0x04,0x08,0x10,0x20}, // 92 '\'
    {0x00,0x41,0x41,0x7F,0x00}, // 93 ']'
    {0x04,0x02,0x01,0x02,0x04}, // 94 '^'
    {0x40,0x40,0x40,0x40,0x40}, // 95 '_'
    {0x00,0x01,0x02,0x04,0x00}, // 96 '`'
    {0x20,0x54,0x54,0x54,0x78}, // 97 'a'
    {0x7F,0x48,0x44,0x44,0x38}, // 98 'b'
    {0x38,0x44,0x44,0x44,0x20}, // 99 'c'
    {0x38,0x44,0x44,0x48,0x7F}, // 100 'd'
    {0x38,0x54,0x54,0x54,0x18}, // 101 'e'
    {0x08,0x7E,0x09,0x01,0x02}, // 102 'f'
    {0x0C,0x52,0x52,0x52,0x3E}, // 103 'g'
    {0x7F,0x08,0x04,0x04,0x78}, // 104 'h'
    {0x00,0x44,0x7D,0x40,0x00}, // 105 'i'
    {0x20,0x40,0x44,0x3D,0x00}, // 106 'j'
    {0x7F,0x10,0x28,0x44,0x00}, // 107 'k'
    {0x00,0x41,0x7F,0x40,0x00}, // 108 'l'
    {0x7C,0x04,0x18,0x04,0x78}, // 109 'm'
    {0x7C,0x08,0x04,0x04,0x78}, // 110 'n'
    {0x38,0x44,0x44,0x44,0x38}, // 111 'o'
    {0x7C,0x14,0x14,0x14,0x08}, // 112 'p'
    {0x08,0x14,0x14,0x18,0x7C}, // 113 'q'
    {0x7C,0x08,0x04,0x04,0x08}, // 114 'r'
    {0x48,0x54,0x54,0x54,0x20}, // 115 's'
    {0x04,0x3F,0x44,0x40,0x20}, // 116 't'
    {0x3C,0x40,0x40,0x20,0x7C}, // 117 'u'
    {0x1C,0x20,0x40,0x20,0x1C}, // 118 'v'
    {0x3C,0x40,0x30,0x40,0x3C}, // 119 'w'
    {0x44,0x28,0x10,0x28,0x44}, // 120 'x'
    {0x0C,0x50,0x50,0x50,0x3C}, // 121 'y'
    {0x44,0x64,0x54,0x4C,0x44}, // 122 'z'
    {0x00,0x08,0x36,0x41,0x00}, // 123 '{'
    {0x00,0x00,0x7F,0x00,0x00}, // 124 '|'
    {0x00,0x41,0x36,0x08,0x00}, // 125 '}'
    {0x08,0x04,0x08,0x10,0x08}  // 126 '~'
};

// -------------------------------------------------------------
// Primitives
// -------------------------------------------------------------

void draw_fill_rect(uint16_t* fb, int x, int y, int w, int h, uint16_t color) {
    if (!fb || x >= DS_SCREEN_W || y >= DS_SCREEN_H) return;
    int x0 = (x < 0) ? 0 : x;
    int y0 = (y < 0) ? 0 : y;
    int x1 = (x + w > DS_SCREEN_W) ? DS_SCREEN_W : x + w;
    int y1 = (y + h > DS_SCREEN_H) ? DS_SCREEN_H : y + h;

    for (int py = y0; py < y1; ++py) {
        uint16_t* line = &fb[py * DS_SCREEN_W + x0];
        for (int px = x0; px < x1; ++px) {
            *line++ = color;
        }
    }
}

void draw_outline_rect(uint16_t* fb, int x, int y, int w, int h, uint16_t color) {
    if (!fb || w <= 0 || h <= 0) return;
    draw_fill_rect(fb, x, y, w, 1, color);             // Top
    draw_fill_rect(fb, x, y + h - 1, w, 1, color);     // Bottom
    draw_fill_rect(fb, x, y, 1, h, color);             // Left
    draw_fill_rect(fb, x + w - 1, y, 1, h, color);     // Right
}

void draw_rounded_panel(uint16_t* fb, int x, int y, int w, int h, uint16_t fill_col, uint16_t border_col) {
    if (w < 4 || h < 4) return;
    draw_fill_rect(fb, x + 1, y + 1, w - 2, h - 2, fill_col);
    draw_fill_rect(fb, x + 1, y, w - 2, 1, border_col);         // Top
    draw_fill_rect(fb, x + 1, y + h - 1, w - 2, 1, border_col); // Bottom
    draw_fill_rect(fb, x, y + 1, 1, h - 2, border_col);         // Left
    draw_fill_rect(fb, x + w - 1, y + 1, 1, h - 2, border_col); // Right
}

void draw_char(uint16_t* fb, int x, int y, char c, uint16_t text_col) {
    if (c < 32 || c > 126) c = '?';
    int idx = c - 32;

    for (int col = 0; col < 5; ++col) {
        int px = x + col;
        if (px < 0 || px >= DS_SCREEN_W) continue;
        uint8_t line = s_font5x7[idx][col];
        for (int row = 0; row < 7; ++row) {
            int py = y + row;
            if (py >= 0 && py < DS_SCREEN_H) {
                if (line & (1 << row)) {
                    fb[py * DS_SCREEN_W + px] = text_col;
                }
            }
        }
    }
}

void draw_string(uint16_t* fb, int x, int y, const char* str, uint16_t text_col) {
    if (!str) return;
    int cur_x = x;
    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            y += 8;
        } else {
            draw_char(fb, cur_x, y, *str, text_col);
            cur_x += 6;
        }
        str++;
    }
}

void draw_string_shadow(uint16_t* fb, int x, int y, const char* str, uint16_t text_col, uint16_t shadow_col) {
    draw_string(fb, x + 1, y + 1, str, shadow_col);
    draw_string(fb, x, y, str, text_col);
}

// 2x Scaled Bold Font for Balatro Numbers & Badges
void draw_char_2x(uint16_t* fb, int x, int y, char c, uint16_t text_col) {
    if (c < 32 || c > 126) c = '?';
    int idx = c - 32;

    for (int col = 0; col < 5; ++col) {
        int px = x + col * 2;
        if (px < 0 || px >= DS_SCREEN_W - 1) continue;
        uint8_t line = s_font5x7[idx][col];
        for (int row = 0; row < 7; ++row) {
            int py = y + row * 2;
            if (py >= 0 && py < DS_SCREEN_H - 1) {
                if (line & (1 << row)) {
                    fb[py * DS_SCREEN_W + px] = text_col;
                    fb[py * DS_SCREEN_W + px + 1] = text_col;
                    fb[(py + 1) * DS_SCREEN_W + px] = text_col;
                    fb[(py + 1) * DS_SCREEN_W + px + 1] = text_col;
                }
            }
        }
    }
}

void draw_string_2x(uint16_t* fb, int x, int y, const char* str, uint16_t text_col) {
    if (!str) return;
    int cur_x = x;
    while (*str) {
        draw_char_2x(fb, cur_x, y, *str, text_col);
        cur_x += 12; // 10 pixels + 2 pixels spacing
        str++;
    }
}

void draw_string_2x_shadow(uint16_t* fb, int x, int y, const char* str, uint16_t text_col, uint16_t shadow_col) {
    draw_string_2x(fb, x + 1, y + 1, str, shadow_col);
    draw_string_2x(fb, x, y, str, text_col);
}

// -------------------------------------------------------------
// Card Rendering (Authentic Balatro 8-Bit Pixel Art Playing Card)
// -------------------------------------------------------------

static void draw_playing_card(uint16_t* fb, int x, int y, const Card* card) {
    // Drop shadow
    draw_fill_rect(fb, x + 2, y + 2, BALATRO_CARD_W, BALATRO_CARD_H, COLOR_CARD_SHADOW);

    int suit_row = 0;
    if (card->suit == SUIT_HEARTS) suit_row = 0;
    else if (card->suit == SUIT_CLUBS) suit_row = 1;
    else if (card->suit == SUIT_DIAMONDS) suit_row = 2;
    else if (card->suit == SUIT_SPADES) suit_row = 3;

    int col = card->rank - 2;
    if (col < 0) col = 0;
    if (col > 12) col = 12;
    int sprite_idx = suit_row * 13 + col;
    const uint16_t* sprite = s_balatro_cards[sprite_idx];

    // Blit authentic Balatro card sprite
    for (int py = 0; py < BALATRO_CARD_H; ++py) {
        int sy = y + py;
        if (sy < 0 || sy >= DS_SCREEN_H) continue;
        uint16_t* row_ptr = &fb[sy * DS_SCREEN_W];
        for (int px = 0; px < BALATRO_CARD_W; ++px) {
            int sx = x + px;
            if (sx < 0 || sx >= DS_SCREEN_W) continue;
            uint16_t color = sprite[py * BALATRO_CARD_W + px];
            if (color != 0x0000) {
                row_ptr[sx] = color;
            }
        }
    }

    // Glowing double border if selected
    if (card->is_selected) {
        draw_outline_rect(fb, x - 1, y - 1, BALATRO_CARD_W + 2, BALATRO_CARD_H + 2, COLOR_CARD_SELECTED);
        draw_outline_rect(fb, x - 2, y - 2, BALATRO_CARD_W + 4, BALATRO_CARD_H + 4, COLOR_CARD_SELECTED);
    }
}

// -------------------------------------------------------------
// Top Screen Rendering (Status, Chips x Mult, Blind Chip, Jokers)
// -------------------------------------------------------------

void renderer_draw_top_screen(uint16_t* fb, const GameSession* game) {
    if (!fb || !game) return;

    // Fill casino green felt background
    draw_fill_rect(fb, 0, 0, DS_SCREEN_W, DS_SCREEN_H, COLOR_TABLE_BG);
    draw_outline_rect(fb, 1, 1, DS_SCREEN_W - 2, DS_SCREEN_H - 2, COLOR_TABLE_ACCENT);

    // If in Victory State, show Celebration Overlay!
    if (game->current_state == STATE_ROUND_VICTORY) {
        draw_rounded_panel(fb, 14, 18, 228, 156, COLOR_BOX_DARK, COLOR_GOLD_COIN);
        draw_outline_rect(fb, 16, 20, 224, 152, COLOR_GOLD_COIN);

        draw_string_shadow(fb, 54, 30, "*** ROUND CLEARED! ***", COLOR_GOLD_COIN, COLOR_BOX_DARK);
        draw_string_2x_shadow(fb, 76, 44, "VICTORY", COLOR_TEXT_WHITE, COLOR_BOX_DARK);

        char pay_str[32];
        sprintf(pay_str, "CASH OUT: +$%d", game->round_payout);
        draw_string_2x_shadow(fb, 42, 68, pay_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

        int base_rew = (game->current_blind == BLIND_SMALL) ? 3 : (game->current_blind == BLIND_BIG ? 4 : 5);
        char b1[48], b2[48], b3[48];
        sprintf(b1, "Base Blind Payout:    +$%d", base_rew);
        sprintf(b2, "Leftover Hands Bonus: +$%d", game->hands_left);
        sprintf(b3, "Total Dollars:         $%d", game->dollars);
        draw_string(fb, 36, 96, b1, COLOR_TEXT_WHITE);
        draw_string(fb, 36, 108, b2, COLOR_TEXT_WHITE);
        draw_string(fb, 36, 120, b3, COLOR_GOLD_COIN);

        draw_string_shadow(fb, 38, 148, "Tap [NEXT BLIND >>] on Bottom Screen", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // If in Game Over State, show Game Over Overlay!
    if (game->current_state == STATE_GAME_OVER) {
        draw_rounded_panel(fb, 14, 18, 228, 156, COLOR_BOX_DARK, COLOR_MULT_RED);
        draw_outline_rect(fb, 16, 20, 224, 152, COLOR_MULT_RED);

        draw_string_shadow(fb, 62, 30, "*** GAME OVER ***", COLOR_MULT_RED, COLOR_BOX_DARK);
        draw_string_2x_shadow(fb, 66, 46, "DEFEATED", COLOR_MULT_RED, COLOR_BOX_DARK);

        draw_string(fb, 44, 72, "You ran out of hands to play!", COLOR_TEXT_WHITE);

        char f_score[48];
        sprintf(f_score, "Final: %lld  /  Goal: %lld", (long long)game->current_round_score, (long long)game->target_score);
        draw_string(fb, 38, 92, f_score, COLOR_TEXT_WHITE);

        char f_ante[32];
        sprintf(f_ante, "Reached: Ante %d / 8", game->ante);
        draw_string(fb, 64, 110, f_ante, COLOR_GOLD_COIN);

        draw_string_shadow(fb, 38, 148, "Tap [RESTART RUN] on Bottom Screen", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // --- Active Round View ---
    // Top Title Bar
    draw_rounded_panel(fb, 76, 2, 104, 14, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string_shadow(fb, 90, 5, "* BALATRO *", COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // Authentic Blind Poker Chip (34x34) at (6, 20)
    const uint16_t* chip_sprite = s_chip_small_blind;
    if (game->current_blind == BLIND_BIG) chip_sprite = s_chip_big_blind;
    else if (game->current_blind == BLIND_BOSS) chip_sprite = s_chip_boss_blind;

    for (int py = 0; py < BALATRO_CHIP_SIZE; ++py) {
        int sy = 20 + py;
        if (sy < 0 || sy >= DS_SCREEN_H) continue;
        uint16_t* row_ptr = &fb[sy * DS_SCREEN_W];
        for (int px = 0; px < BALATRO_CHIP_SIZE; ++px) {
            int sx = 8 + px;
            if (sx < 0 || sx >= DS_SCREEN_W) continue;
            uint16_t color = chip_sprite[py * BALATRO_CHIP_SIZE + px];
            if (color != 0x0000) {
                row_ptr[sx] = color;
            }
        }
    }

    // Ante & Blind Badges
    draw_rounded_panel(fb, 46, 20, 54, 14, COLOR_RGB(28, 12, 4), COLOR_DIAMOND_ORANGE);
    char ante_str[16];
    sprintf(ante_str, "ANTE %d/8", game->ante);
    draw_string(fb, 50, 23, ante_str, COLOR_TEXT_WHITE);

    const char* b_title = (game->current_blind == BLIND_SMALL ? "SMALL BLIND" :
                          (game->current_blind == BLIND_BIG   ? "BIG BLIND"   : "BOSS BLIND"));
    draw_string_shadow(fb, 48, 38, b_title, COLOR_GOLD_COIN, COLOR_BOX_DARK);

    int reward_dollars = (game->current_blind == BLIND_SMALL) ? 3 : (game->current_blind == BLIND_BIG ? 4 : 5);
    char rew_str[16];
    sprintf(rew_str, "Reward: +$%d", reward_dollars);
    draw_string(fb, 48, 48, rew_str, COLOR_RGB(16, 24, 18));

    // Target Score Panel
    draw_rounded_panel(fb, 144, 18, 106, 38, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 150, 22, "SCORE AT LEAST:", COLOR_RGB(16, 22, 18));
    char goal_str[32];
    sprintf(goal_str, "%lld", (long long)game->target_score);
    draw_string_2x_shadow(fb, 152, 33, goal_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // Round Progress / Current Score
    draw_rounded_panel(fb, 8, 58, 240, 20, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 14, 64, "ROUND SCORE:", COLOR_RGB(16, 20, 18));
    char score_str[32];
    sprintf(score_str, "%lld / %lld", (long long)game->current_round_score, (long long)game->target_score);
    draw_string_shadow(fb, 94, 64, score_str, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Subtle Progress Gauge inside score bar
    if (game->target_score > 0) {
        int bar_w = (int)((game->current_round_score * 50) / game->target_score);
        if (bar_w > 50) bar_w = 50;
        if (bar_w > 0) {
            draw_fill_rect(fb, 194, 63, bar_w, 10, COLOR_GOLD_COIN);
        }
        draw_outline_rect(fb, 193, 62, 52, 12, COLOR_BOX_BORDER);
    }

    // Jokers Tray (5 mini card slots)
    // Displayed in a neat row if any are equipped
    // (Jokers are drawn if equipped)

    // Iconic Chips & Mult Calculator Box
    draw_rounded_panel(fb, 6, 82, 244, 82, COLOR_BOX_DARK, COLOR_BOX_BORDER);

    // Determine values to show: Live Selection Preview if cards selected, else last scored hand
    int64_t disp_chips = 0;
    double disp_mult = 1.0;
    const char* hand_display_name = "SELECT 1-5 CARDS";
    bool is_previewing = false;

    if (game->preview_eval.type != HAND_NONE) {
        disp_chips = game->preview_score.chips;
        disp_mult = game->preview_score.mult;
        hand_display_name = poker_hand_name(game->preview_eval.type);
        is_previewing = true;
    } else if (game->last_score.final_score > 0) {
        disp_chips = game->last_score.chips;
        disp_mult = game->last_score.mult;
        hand_display_name = poker_hand_name(game->last_eval.type);
    }

    // Blue Chips Box (Left)
    draw_rounded_panel(fb, 12, 88, 98, 44, COLOR_CHIPS_BLUE, COLOR_RGB(8, 24, 31));
    draw_outline_rect(fb, 13, 89, 96, 42, COLOR_RGB(16, 26, 31));
    draw_string(fb, 18, 92, "CHIPS", COLOR_RGB(18, 26, 31));
    char chips_val[16];
    sprintf(chips_val, "%lld", (long long)disp_chips);
    draw_string_2x_shadow(fb, 22, 106, chips_val, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Center Gold "X" multiplication sign
    draw_string_2x_shadow(fb, 118, 102, "X", COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // Red Mult Box (Right)
    draw_rounded_panel(fb, 146, 88, 98, 44, COLOR_MULT_RED, COLOR_RGB(31, 10, 12));
    draw_outline_rect(fb, 147, 89, 96, 42, COLOR_RGB(31, 18, 20));
    draw_string(fb, 152, 92, "MULT", COLOR_RGB(31, 22, 24));
    char mult_val[16];
    sprintf(mult_val, "%.1f", disp_mult);
    draw_string_2x_shadow(fb, 156, 106, mult_val, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Hand Name Badge (Shows live detected poker hand name!)
    uint16_t h_col = is_previewing ? COLOR_GOLD_COIN : COLOR_RGB(14, 20, 16);
    draw_string_shadow(fb, 14, 138, hand_display_name, h_col, COLOR_BOX_DARK);

    if (is_previewing) {
        char est_str[32];
        int64_t est_total = (int64_t)(disp_chips * disp_mult);
        sprintf(est_str, "EST: %lld PTS", (long long)est_total);
        draw_string_shadow(fb, 140, 138, est_str, COLOR_TEXT_WHITE, COLOR_BOX_DARK);
    } else {
        draw_string(fb, 140, 138, "Max 5 cards", COLOR_RGB(12, 16, 14));
    }

    // Bottom Stats Bar (Money, Hands, Discards)
    draw_rounded_panel(fb, 6, 168, 74, 18, COLOR_RGB(20, 16, 4), COLOR_GOLD_COIN);
    char money_str[16];
    sprintf(money_str, "$%d", game->dollars);
    draw_string_shadow(fb, 14, 173, money_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

    draw_rounded_panel(fb, 86, 168, 78, 18, COLOR_CHIPS_BLUE, COLOR_BOX_BORDER);
    char hands_str[16];
    sprintf(hands_str, "%d HANDS", game->hands_left);
    draw_string(fb, 92, 173, hands_str, COLOR_TEXT_WHITE);

    draw_rounded_panel(fb, 170, 168, 80, 18, COLOR_MULT_RED, COLOR_BOX_BORDER);
    char disc_str[16];
    sprintf(disc_str, "%d DISCARDS", game->discards_left);
    draw_string(fb, 174, 173, disc_str, COLOR_TEXT_WHITE);
}

// -------------------------------------------------------------
// Bottom Screen Rendering (The Hand, Stylus Touch Buttons)
// -------------------------------------------------------------

void renderer_draw_bottom_screen(uint16_t* fb, const GameSession* game) {
    if (!fb || !game) return;

    // Rich Green Felt Table Background
    draw_fill_rect(fb, 0, 0, DS_SCREEN_W, DS_SCREEN_H, COLOR_TABLE_BG);
    draw_outline_rect(fb, 1, 1, DS_SCREEN_W - 2, DS_SCREEN_H - 2, COLOR_TABLE_ACCENT);

    // If Victory State: Show Big [NEXT BLIND >>] Button
    if (game->current_state == STATE_ROUND_VICTORY) {
        draw_rounded_panel(fb, 24, 130, 208, 52, COLOR_RGB(4, 26, 10), COLOR_GOLD_COIN);
        draw_outline_rect(fb, 26, 132, 204, 48, COLOR_GOLD_COIN);
        draw_string_2x_shadow(fb, 48, 146, "NEXT BLIND >>", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // If Game Over State: Show Big [RESTART RUN] Button
    if (game->current_state == STATE_GAME_OVER) {
        draw_rounded_panel(fb, 24, 130, 208, 52, COLOR_RGB(28, 6, 6), COLOR_GOLD_COIN);
        draw_outline_rect(fb, 26, 132, 204, 48, COLOR_GOLD_COIN);
        draw_string_2x_shadow(fb, 52, 146, "RESTART RUN", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // Active Round Controls:
    // Sort Buttons
    draw_rounded_panel(fb, 8, 6, 82, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 14, 11, "SORT: RANK", COLOR_TEXT_WHITE);

    draw_rounded_panel(fb, 96, 6, 82, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 102, 11, "SORT: SUIT", COLOR_TEXT_WHITE);

    // Hand card count badge
    char hand_count_str[16];
    sprintf(hand_count_str, "HAND %d/%d", game->deck.hand.count, game->deck.hand_size_limit);
    draw_string(fb, 188, 11, hand_count_str, COLOR_RGB(16, 22, 18));

    // Draw Authentic Balatro Cards spread out horizontally
    int start_x = 8;
    int base_y = 66;

    for (int i = 0; i < game->deck.hand.count; ++i) {
        int cx = start_x + (i * 30);
        // If selected, card LIFTS UP by 14 pixels
        int cy = game->deck.hand.cards[i].is_selected ? (base_y - 14) : base_y;

        draw_playing_card(fb, cx, cy, &game->deck.hand.cards[i]);

        // Draw card index number below
        char num_buf[4];
        sprintf(num_buf, "%d", i + 1);
        uint16_t num_col = game->deck.hand.cards[i].is_selected ? COLOR_CARD_SELECTED : COLOR_RGB(10, 18, 14);
        draw_string(fb, cx + 10, base_y + 40, num_buf, num_col);
    }

    // Bottom Touch Action Buttons
    const Card* sel_cards[5];
    int sel_count = deck_get_selected_cards((DeckManager*)&game->deck, sel_cards);

    // PLAY HAND Button (Big Blue)
    uint16_t play_bg = (sel_count > 0) ? COLOR_PLAY_BTN : COLOR_RGB(2, 8, 14);
    draw_rounded_panel(fb, 12, 136, 112, 44, play_bg, COLOR_RGB(10, 24, 31));
    draw_outline_rect(fb, 13, 137, 110, 42, (sel_count > 0) ? COLOR_CHIPS_BLUE : COLOR_RGB(4, 12, 18));
    if (sel_count > 0) {
        draw_string_shadow(fb, 24, 148, "PLAY HAND", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        char est_pts[24];
        int64_t est_total = (int64_t)(game->preview_score.chips * game->preview_score.mult);
        sprintf(est_pts, "%lld PTS", (long long)est_total);
        draw_string_shadow(fb, 24, 160, est_pts, COLOR_GOLD_COIN, COLOR_BOX_DARK);
    } else {
        draw_string(fb, 18, 154, "SELECT CARDS", COLOR_RGB(12, 16, 18));
    }

    // DISCARD Button (Big Red)
    bool can_discard = (game->discards_left > 0 && sel_count > 0);
    uint16_t disc_bg = can_discard ? COLOR_DISCARD_BTN : COLOR_RGB(14, 3, 4);
    draw_rounded_panel(fb, 132, 136, 112, 44, disc_bg, COLOR_RGB(31, 10, 12));
    draw_outline_rect(fb, 133, 137, 110, 42, can_discard ? COLOR_MULT_RED : COLOR_RGB(18, 4, 6));
    if (game->discards_left <= 0) {
        draw_string(fb, 142, 154, "NO DISCARDS", COLOR_RGB(18, 10, 10));
    } else if (sel_count > 0) {
        draw_string_shadow(fb, 156, 148, "DISCARD", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        char disc_left[24];
        sprintf(disc_left, "(%d LEFT)", game->discards_left);
        draw_string_shadow(fb, 156, 160, disc_left, COLOR_TEXT_WHITE, COLOR_BOX_DARK);
    } else {
        draw_string(fb, 156, 154, "DISCARD", COLOR_RGB(16, 12, 12));
    }
}
