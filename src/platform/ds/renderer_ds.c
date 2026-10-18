#include "platform/renderer_ds.h"
#include "gfx/balatro_deck_gfx.h"
#include "gfx/balatro_chips_gfx.h"
#include "gfx/balatro_consumables_gfx.h"
#include "gfx/balatro_jokers_gfx.h"
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
    {0x7F,0x09,0x09,0x09,0x01}, // 70 'F'
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
        cur_x += 12;
        str++;
    }
}

void draw_string_2x_shadow(uint16_t* fb, int x, int y, const char* str, uint16_t text_col, uint16_t shadow_col) {
    draw_string_2x(fb, x + 1, y + 1, str, shadow_col);
    draw_string_2x(fb, x, y, str, text_col);
}

// -------------------------------------------------------------
// Card & Sprite Blitting Helpers
// -------------------------------------------------------------

void renderer_draw_card_sprite(uint16_t* fb, int x, int y, const Card* card, bool debuffed) {
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

    for (int py = 0; py < BALATRO_CARD_H; ++py) {
        int sy = y + py;
        if (sy < 0 || sy >= DS_SCREEN_H) continue;
        uint16_t* row_ptr = &fb[sy * DS_SCREEN_W];
        for (int px = 0; px < BALATRO_CARD_W; ++px) {
            int sx = x + px;
            if (sx < 0 || sx >= DS_SCREEN_W) continue;
            uint16_t color = sprite[py * BALATRO_CARD_W + px];
            if (color != 0x0000) {
                if (debuffed && (px == py || px == (BALATRO_CARD_W - 1 - py))) {
                    row_ptr[sx] = COLOR_MULT_RED; // Red debuff cross
                } else {
                    row_ptr[sx] = color;
                }
            }
        }
    }

    // Glowing double border if selected
    if (card->is_selected) {
        draw_outline_rect(fb, x - 1, y - 1, BALATRO_CARD_W + 2, BALATRO_CARD_H + 2, COLOR_CARD_SELECTED);
        draw_outline_rect(fb, x - 2, y - 2, BALATRO_CARD_W + 4, BALATRO_CARD_H + 4, COLOR_CARD_SELECTED);
    }
}

void renderer_draw_consumable_sprite(uint16_t* fb, int x, int y, const Consumable* c, bool is_selected) {
    if (!c || c->type == CONSUMABLE_NONE) return;
    draw_fill_rect(fb, x + 2, y + 2, BALATRO_CONS_W, BALATRO_CONS_H, COLOR_CARD_SHADOW);

    int idx = c->gfx_index;
    if (idx < 0 || idx >= BALATRO_CONSUMABLE_COUNT) idx = 0;
    const uint16_t* sprite = s_balatro_consumables[idx];

    for (int py = 0; py < BALATRO_CONS_H; ++py) {
        int sy = y + py;
        if (sy < 0 || sy >= DS_SCREEN_H) continue;
        uint16_t* row_ptr = &fb[sy * DS_SCREEN_W];
        for (int px = 0; px < BALATRO_CONS_W; ++px) {
            int sx = x + px;
            if (sx < 0 || sx >= DS_SCREEN_W) continue;
            uint16_t color = sprite[py * BALATRO_CONS_W + px];
            if (color != 0x0000) row_ptr[sx] = color;
        }
    }

    if (is_selected) {
        draw_outline_rect(fb, x - 1, y - 1, BALATRO_CONS_W + 2, BALATRO_CONS_H + 2, COLOR_GOLD_COIN);
    }
}

void renderer_draw_joker_sprite(uint16_t* fb, int x, int y, const Joker* j, bool is_selected) {
    if (!j) return;
    draw_fill_rect(fb, x + 2, y + 2, BALATRO_JK_W, BALATRO_JK_H, COLOR_CARD_SHADOW);

    int idx = j->id % BALATRO_JOKER_COUNT;
    const uint16_t* sprite = s_balatro_jokers[idx];

    for (int py = 0; py < BALATRO_JK_H; ++py) {
        int sy = y + py;
        if (sy < 0 || sy >= DS_SCREEN_H) continue;
        uint16_t* row_ptr = &fb[sy * DS_SCREEN_W];
        for (int px = 0; px < BALATRO_JK_W; ++px) {
            int sx = x + px;
            if (sx < 0 || sx >= DS_SCREEN_W) continue;
            uint16_t color = sprite[py * BALATRO_JK_W + px];
            if (color != 0x0000) row_ptr[sx] = color;
        }
    }

    if (is_selected) {
        draw_outline_rect(fb, x - 1, y - 1, BALATRO_JK_W + 2, BALATRO_JK_H + 2, COLOR_CARD_SELECTED);
    }
}

// -------------------------------------------------------------
// Top Screen Rendering (Status, Chips x Mult, Blind Chip, Jokers, Tarots)
// -------------------------------------------------------------

void renderer_draw_top_screen(uint16_t* fb, const GameSession* game) {
    if (!fb || !game) return;

    // Casino Green Felt Table Background
    draw_fill_rect(fb, 0, 0, DS_SCREEN_W, DS_SCREEN_H, COLOR_TABLE_BG);
    draw_outline_rect(fb, 1, 1, DS_SCREEN_W - 2, DS_SCREEN_H - 2, COLOR_TABLE_ACCENT);

    // --- State: Victory / Cash Out Overlay ---
    if (game->current_state == STATE_ROUND_VICTORY) {
        draw_rounded_panel(fb, 14, 16, 228, 160, COLOR_BOX_DARK, COLOR_GOLD_COIN);
        draw_outline_rect(fb, 16, 18, 224, 156, COLOR_GOLD_COIN);

        draw_string_shadow(fb, 54, 26, "*** ROUND CLEARED! ***", COLOR_GOLD_COIN, COLOR_BOX_DARK);
        draw_string_2x_shadow(fb, 76, 40, "VICTORY", COLOR_TEXT_WHITE, COLOR_BOX_DARK);

        char pay_str[32];
        sprintf(pay_str, "CASH OUT: +$%d", game->round_payout);
        draw_string_2x_shadow(fb, 42, 64, pay_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

        int base_rew = (game->current_blind == BLIND_SMALL) ? 3 : (game->current_blind == BLIND_BIG ? 4 : 5);
        int interest = game->dollars / 5;
        if (interest > 5) interest = 5;

        char b1[48], b2[48], b3[48], b4[48];
        sprintf(b1, "Base Blind Payout:    +$%d", base_rew);
        sprintf(b2, "Leftover Hands Bonus: +$%d", game->hands_left);
        sprintf(b3, "Interest Earned:      +$%d", interest);
        sprintf(b4, "Total Money:           $%d", game->dollars);

        draw_string(fb, 32, 92, b1, COLOR_TEXT_WHITE);
        draw_string(fb, 32, 104, b2, COLOR_TEXT_WHITE);
        draw_string(fb, 32, 116, b3, COLOR_TEXT_WHITE);
        draw_string(fb, 32, 128, b4, COLOR_GOLD_COIN);

        draw_string_shadow(fb, 36, 150, "Tap [GO TO SHOP >>] on Bottom Screen", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // --- State: Game Over Overlay ---
    if (game->current_state == STATE_GAME_OVER) {
        draw_rounded_panel(fb, 14, 16, 228, 160, COLOR_BOX_DARK, COLOR_MULT_RED);
        draw_outline_rect(fb, 16, 18, 224, 156, COLOR_MULT_RED);

        draw_string_shadow(fb, 62, 28, "*** GAME OVER ***", COLOR_MULT_RED, COLOR_BOX_DARK);
        draw_string_2x_shadow(fb, 66, 44, "DEFEATED", COLOR_MULT_RED, COLOR_BOX_DARK);

        draw_string(fb, 44, 70, "You ran out of hands to play!", COLOR_TEXT_WHITE);

        char f_score[48];
        sprintf(f_score, "Final: %lld  /  Goal: %lld", (long long)game->current_round_score, (long long)game->target_score);
        draw_string(fb, 36, 92, f_score, COLOR_TEXT_WHITE);

        char f_ante[32];
        sprintf(f_ante, "Reached: Ante %d / 8", game->ante);
        draw_string(fb, 64, 112, f_ante, COLOR_GOLD_COIN);

        draw_string_shadow(fb, 36, 150, "Tap [RESTART RUN] on Bottom Screen", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // --- State: Shop Top Screen ---
    if (game->current_state == STATE_SHOP || game->current_state == STATE_BOOSTER_OPEN) {
        draw_rounded_panel(fb, 70, 2, 116, 16, COLOR_BOX_DARK, COLOR_BOX_BORDER);
        draw_string_shadow(fb, 80, 5, "* BALATRO SHOP *", COLOR_GOLD_COIN, COLOR_BOX_DARK);

        // Money Badge
        draw_rounded_panel(fb, 8, 22, 94, 24, COLOR_RGB(24, 18, 4), COLOR_GOLD_COIN);
        char mon_str[24];
        sprintf(mon_str, "$%d CASH", game->dollars);
        draw_string_shadow(fb, 16, 30, mon_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

        // Jokers Info
        char jk_info[32];
        sprintf(jk_info, "JOKERS: %d/5", game->joker_count);
        draw_string(fb, 114, 24, jk_info, COLOR_TEXT_WHITE);

        char cs_info[32];
        sprintf(cs_info, "CONSUMABLES: %d/2", game->consumable_count);
        draw_string(fb, 114, 36, cs_info, COLOR_TEXT_WHITE);

        // Draw Owned Jokers in a neat row
        for (int i = 0; i < 5; ++i) {
            int jx = 8 + i * 49;
            int jy = 54;
            if (i < game->joker_count && game->jokers[i]) {
                renderer_draw_joker_sprite(fb, jx, jy, game->jokers[i], false);
                char jname[12];
                strncpy(jname, game->jokers[i]->name, 7);
                jname[7] = '\0';
                draw_string(fb, jx, jy + 38, jname, COLOR_TEXT_WHITE);
            } else {
                draw_rounded_panel(fb, jx, jy, 26, 36, COLOR_RGB(2, 4, 3), COLOR_BOX_BORDER);
                draw_string(fb, jx + 8, jy + 14, "+", COLOR_RGB(8, 14, 10));
            }
        }

        // Draw Owned Consumables (Tarots / Planets)
        for (int c = 0; c < 2; ++c) {
            int cx = 156 + c * 46;
            int cy = 114;
            if (c < game->consumable_count && game->consumables[c].type != CONSUMABLE_NONE) {
                renderer_draw_consumable_sprite(fb, cx, cy, &game->consumables[c], false);
                char cname[12];
                strncpy(cname, game->consumables[c].name, 7);
                cname[7] = '\0';
                draw_string(fb, cx, cy + 38, cname, COLOR_GOLD_COIN);
            } else {
                draw_rounded_panel(fb, cx, cy, 26, 36, COLOR_RGB(2, 4, 3), COLOR_BOX_BORDER);
                draw_string(fb, cx + 8, cy + 14, "T", COLOR_RGB(8, 14, 10));
            }
        }

        draw_string(fb, 14, 168, "Manage or inspect your deck & inventory", COLOR_RGB(16, 22, 18));
        return;
    }

    // --- State: Active Round & Scoring Top Screen ---
    // 1. Jokers Tray across the top (5 slots)
    bool jokers_firing = (game->current_state == STATE_ROUND_SCORING &&
                          game->scoring_step == (game->last_eval.scoring_count + 2));

    for (int j = 0; j < 5; ++j) {
        int jx = 8 + j * 49;
        int jy = 2;
        if (j < game->joker_count && game->jokers[j]) {
            renderer_draw_joker_sprite(fb, jx, jy, game->jokers[j], jokers_firing);
        } else {
            draw_rounded_panel(fb, jx, jy, 26, 36, COLOR_RGB(2, 4, 3), COLOR_BOX_BORDER);
            draw_string(fb, jx + 8, jy + 14, "+", COLOR_RGB(8, 14, 10));
        }
    }

    // 2. Blind Poker Chip (34x34) at (6, 40)
    const uint16_t* chip_sprite = s_chip_small_blind;
    if (game->current_blind == BLIND_BIG) chip_sprite = s_chip_big_blind;
    else if (game->current_blind == BLIND_BOSS) chip_sprite = s_chip_boss_blind;

    for (int py = 0; py < BALATRO_CHIP_SIZE; ++py) {
        int sy = 40 + py;
        if (sy < 0 || sy >= DS_SCREEN_H) continue;
        uint16_t* row_ptr = &fb[sy * DS_SCREEN_W];
        for (int px = 0; px < BALATRO_CHIP_SIZE; ++px) {
            int sx = 6 + px;
            if (sx < 0 || sx >= DS_SCREEN_W) continue;
            uint16_t color = chip_sprite[py * BALATRO_CHIP_SIZE + px];
            if (color != 0x0000) row_ptr[sx] = color;
        }
    }

    // Ante Badge & Blind Title
    draw_rounded_panel(fb, 44, 40, 52, 12, COLOR_RGB(28, 12, 4), COLOR_DIAMOND_ORANGE);
    char ante_str[16];
    sprintf(ante_str, "ANTE %d/8", game->ante);
    draw_string(fb, 47, 42, ante_str, COLOR_TEXT_WHITE);

    const char* b_title = (game->current_blind == BLIND_SMALL ? "SMALL BLIND" :
                          (game->current_blind == BLIND_BIG   ? "BIG BLIND"   : "BOSS BLIND"));
    draw_string_shadow(fb, 44, 54, b_title, COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // If Boss Blind, display boss name and debuff description!
    if (game->current_blind == BLIND_BOSS) {
        BossBlindInfo binfo = boss_blind_get_info(game->current_boss);
        char boss_desc[48];
        sprintf(boss_desc, "%s: %s", binfo.name, binfo.description);
        draw_string(fb, 44, 66, boss_desc, COLOR_DIAMOND_ORANGE);
    } else {
        int reward_dollars = (game->current_blind == BLIND_SMALL) ? 3 : 4;
        char rew_str[16];
        sprintf(rew_str, "Reward: +$%d", reward_dollars);
        draw_string(fb, 44, 66, rew_str, COLOR_RGB(16, 24, 18));
    }

    // Target Score Panel at (144, 40)
    draw_rounded_panel(fb, 144, 40, 106, 36, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 150, 43, "SCORE AT LEAST:", COLOR_RGB(16, 22, 18));
    char goal_str[32];
    sprintf(goal_str, "%lld", (long long)game->target_score);
    draw_string_2x_shadow(fb, 150, 53, goal_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // 3. Round Progress Bar at (8, 78)
    draw_rounded_panel(fb, 8, 78, 240, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 14, 83, "ROUND SCORE:", COLOR_RGB(16, 20, 18));
    char score_str[32];
    sprintf(score_str, "%lld / %lld", (long long)game->current_round_score, (long long)game->target_score);
    draw_string_shadow(fb, 86, 83, score_str, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    if (game->target_score > 0) {
        int bar_w = (int)((game->current_round_score * 48) / game->target_score);
        if (bar_w > 48) bar_w = 48;
        if (bar_w > 0) draw_fill_rect(fb, 196, 81, bar_w, 12, COLOR_GOLD_COIN);
        draw_outline_rect(fb, 195, 80, 50, 14, COLOR_BOX_BORDER);
    }

    // 4. Iconic Chips & Mult Calculator Box at (6, 98)
    draw_rounded_panel(fb, 6, 98, 244, 68, COLOR_BOX_DARK, COLOR_BOX_BORDER);

    int64_t disp_chips = 0;
    double disp_mult = 1.0;
    const char* hand_display_name = "SELECT 1-5 CARDS";
    bool is_previewing = false;

    if (game->current_state == STATE_ROUND_SCORING) {
        disp_chips = game->last_score.chips;
        disp_mult = game->last_score.mult;
        hand_display_name = poker_hand_name(game->last_eval.type);
    } else if (game->preview_eval.type != HAND_NONE) {
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
    draw_rounded_panel(fb, 12, 102, 98, 40, COLOR_CHIPS_BLUE, COLOR_RGB(8, 24, 31));
    draw_outline_rect(fb, 13, 103, 96, 38, COLOR_RGB(16, 26, 31));
    draw_string(fb, 16, 105, "CHIPS", COLOR_RGB(18, 26, 31));
    char chips_val[16];
    sprintf(chips_val, "%lld", (long long)disp_chips);
    draw_string_2x_shadow(fb, 18, 117, chips_val, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Center Gold "X"
    draw_string_2x_shadow(fb, 118, 114, "X", COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // Red Mult Box (Right)
    draw_rounded_panel(fb, 146, 102, 98, 40, COLOR_MULT_RED, COLOR_RGB(31, 10, 12));
    draw_outline_rect(fb, 147, 103, 96, 38, COLOR_RGB(31, 18, 20));
    draw_string(fb, 150, 105, "MULT", COLOR_RGB(31, 22, 24));
    char mult_val[16];
    sprintf(mult_val, "%.1f", disp_mult);
    draw_string_2x_shadow(fb, 152, 117, mult_val, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Hand Name Badge
    uint16_t h_col = (is_previewing || game->current_state == STATE_ROUND_SCORING) ? COLOR_GOLD_COIN : COLOR_RGB(14, 20, 16);
    draw_string_shadow(fb, 14, 148, hand_display_name, h_col, COLOR_BOX_DARK);

    if (game->current_state == STATE_ROUND_SCORING) {
        draw_string_shadow(fb, 140, 148, "SCORING HAND...", COLOR_GOLD_COIN, COLOR_BOX_DARK);
    } else if (is_previewing) {
        char est_str[32];
        int64_t est_total = (int64_t)(disp_chips * disp_mult);
        sprintf(est_str, "EST: %lld PTS", (long long)est_total);
        draw_string_shadow(fb, 140, 148, est_str, COLOR_TEXT_WHITE, COLOR_BOX_DARK);
    } else {
        draw_string(fb, 140, 148, "Max 5 cards", COLOR_RGB(12, 16, 14));
    }

    // 5. Bottom Stats Bar (Money, Hands, Discards)
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
// Bottom Screen Rendering (The Hand, Stylus Touch Buttons, Shop, Booster)
// -------------------------------------------------------------

void renderer_draw_bottom_screen(uint16_t* fb, const GameSession* game) {
    if (!fb || !game) return;

    // Green Felt Table Background
    draw_fill_rect(fb, 0, 0, DS_SCREEN_W, DS_SCREEN_H, COLOR_TABLE_BG);
    draw_outline_rect(fb, 1, 1, DS_SCREEN_W - 2, DS_SCREEN_H - 2, COLOR_TABLE_ACCENT);

    // --- State: Victory State Button ---
    if (game->current_state == STATE_ROUND_VICTORY) {
        draw_rounded_panel(fb, 24, 130, 208, 52, COLOR_RGB(4, 26, 10), COLOR_GOLD_COIN);
        draw_outline_rect(fb, 26, 132, 204, 48, COLOR_GOLD_COIN);
        draw_string_2x_shadow(fb, 44, 146, "GO TO SHOP >>", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // --- State: Game Over State Button ---
    if (game->current_state == STATE_GAME_OVER) {
        draw_rounded_panel(fb, 24, 130, 208, 52, COLOR_RGB(28, 6, 6), COLOR_GOLD_COIN);
        draw_outline_rect(fb, 26, 132, 204, 48, COLOR_GOLD_COIN);
        draw_string_2x_shadow(fb, 52, 146, "RESTART RUN", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // --- State: The Shop ---
    if (game->current_state == STATE_SHOP) {
        if (!game->shop.show_inventory) {
            // Shop Card Slot 0 (Joker)
            int s0_x = 8;
            int s0_y = 10;
            draw_rounded_panel(fb, s0_x, s0_y, 70, 78, COLOR_BOX_DARK, COLOR_BOX_BORDER);
            if (!game->shop.slots[0].is_sold) {
                renderer_draw_joker_sprite(fb, s0_x + 22, s0_y + 6, &game->shop.slots[0].data.joker, false);
                draw_string(fb, s0_x + 4, s0_y + 46, game->shop.slots[0].data.joker.name, COLOR_TEXT_WHITE);

                bool can_buy = (game->dollars >= game->shop.slots[0].cost && game->joker_count < MAX_JOKERS_SLOTS);
                uint16_t b0_bg = can_buy ? COLOR_RGB(4, 18, 8) : COLOR_RGB(8, 8, 8);
                uint16_t b0_bd = can_buy ? COLOR_GOLD_COIN : COLOR_RGB(16, 16, 16);
                draw_rounded_panel(fb, s0_x + 4, s0_y + 58, 62, 16, b0_bg, b0_bd);

                if (game->joker_count >= MAX_JOKERS_SLOTS) {
                    draw_string(fb, s0_x + 8, s0_y + 62, "FULL (5/5)", COLOR_RGB(20, 16, 16));
                } else if (game->dollars < game->shop.slots[0].cost) {
                    char c0_str[24];
                    sprintf(c0_str, "NEED $%d", game->shop.slots[0].cost);
                    draw_string(fb, s0_x + 6, s0_y + 62, c0_str, COLOR_RGB(20, 16, 16));
                } else {
                    char c0_str[24];
                    sprintf(c0_str, "BUY: $%d", game->shop.slots[0].cost);
                    draw_string(fb, s0_x + 8, s0_y + 62, c0_str, COLOR_GOLD_COIN);
                }
            } else {
                draw_string(fb, s0_x + 16, s0_y + 36, "SOLD", COLOR_RGB(18, 8, 8));
            }

            // Shop Card Slot 1 (Tarot or Planet)
            int s1_x = 84;
            int s1_y = 10;
            draw_rounded_panel(fb, s1_x, s1_y, 70, 78, COLOR_BOX_DARK, COLOR_BOX_BORDER);
            if (!game->shop.slots[1].is_sold) {
                renderer_draw_consumable_sprite(fb, s1_x + 22, s1_y + 6, &game->shop.slots[1].data.consumable, false);
                draw_string(fb, s1_x + 4, s1_y + 46, game->shop.slots[1].data.consumable.name, COLOR_TEXT_WHITE);

                bool can_buy = (game->dollars >= game->shop.slots[1].cost && game->consumable_count < MAX_CONSUMABLES_SLOTS);
                uint16_t b1_bg = can_buy ? COLOR_RGB(4, 18, 8) : COLOR_RGB(8, 8, 8);
                uint16_t b1_bd = can_buy ? COLOR_GOLD_COIN : COLOR_RGB(16, 16, 16);
                draw_rounded_panel(fb, s1_x + 4, s1_y + 58, 62, 16, b1_bg, b1_bd);

                if (game->consumable_count >= MAX_CONSUMABLES_SLOTS) {
                    draw_string(fb, s1_x + 8, s1_y + 62, "FULL (2/2)", COLOR_RGB(20, 16, 16));
                } else if (game->dollars < game->shop.slots[1].cost) {
                    char c1_str[24];
                    sprintf(c1_str, "NEED $%d", game->shop.slots[1].cost);
                    draw_string(fb, s1_x + 6, s1_y + 62, c1_str, COLOR_RGB(20, 16, 16));
                } else {
                    char c1_str[24];
                    sprintf(c1_str, "BUY: $%d", game->shop.slots[1].cost);
                    draw_string(fb, s1_x + 8, s1_y + 62, c1_str, COLOR_GOLD_COIN);
                }
            } else {
                draw_string(fb, s1_x + 16, s1_y + 36, "SOLD", COLOR_RGB(18, 8, 8));
            }

            // Booster Pack Slot
            int sp_x = 160;
            int sp_y = 10;
            draw_rounded_panel(fb, sp_x, sp_y, 88, 78, COLOR_RGB(14, 8, 22), COLOR_RGB(24, 12, 31));
            if (!game->shop.pack_slot.is_sold) {
                const char* pname = (game->shop.pack_slot.data.pack_type == PACK_ARCANA) ? "Arcana" :
                                    (game->shop.pack_slot.data.pack_type == PACK_CELESTIAL) ? "Celestial" : "Buffoon";
                draw_string_shadow(fb, sp_x + 8, sp_y + 8, pname, COLOR_GOLD_COIN, COLOR_BOX_DARK);
                draw_string(fb, sp_x + 8, sp_y + 22, "Booster Pack", COLOR_TEXT_WHITE);

                bool can_buy = (game->dollars >= 4);
                uint16_t bp_bg = can_buy ? COLOR_RGB(28, 16, 4) : COLOR_RGB(10, 8, 8);
                uint16_t bp_bd = can_buy ? COLOR_GOLD_COIN : COLOR_RGB(16, 16, 16);
                draw_rounded_panel(fb, sp_x + 8, sp_y + 52, 72, 18, bp_bg, bp_bd);

                if (can_buy) {
                    draw_string(fb, sp_x + 14, sp_y + 56, "OPEN ($4)", COLOR_GOLD_COIN);
                } else {
                    draw_string(fb, sp_x + 14, sp_y + 56, "NEED $4", COLOR_RGB(20, 16, 16));
                }
            } else {
                draw_string(fb, sp_x + 22, sp_y + 36, "OPENED", COLOR_RGB(18, 8, 8));
            }

            // Voucher Slot
            int sv_x = 8;
            int sv_y = 94;
            draw_rounded_panel(fb, sv_x, sv_y, 146, 38, COLOR_BOX_DARK, COLOR_BOX_BORDER);
            if (!game->shop.voucher_slot.is_sold) {
                draw_string(fb, sv_x + 6, sv_y + 5, "Voucher: Upgrade", COLOR_GOLD_COIN);
                bool can_vouch = (game->dollars >= game->shop.voucher_slot.cost);
                char v_str[32];
                sprintf(v_str, "%s ($%d)", can_vouch ? "BUY VOUCHER" : "NEED", game->shop.voucher_slot.cost);
                draw_string(fb, sv_x + 6, sv_y + 19, v_str, can_vouch ? COLOR_TEXT_WHITE : COLOR_RGB(18, 14, 14));
            } else {
                draw_string(fb, sv_x + 6, sv_y + 12, "VOUCHER PURCHASED", COLOR_RGB(12, 24, 12));
            }

            // Inventory / Sell Button
            int sinv_x = 160;
            int sinv_y = 94;
            draw_rounded_panel(fb, sinv_x, sinv_y, 88, 38, COLOR_RGB(24, 16, 4), COLOR_GOLD_COIN);
            draw_string_shadow(fb, sinv_x + 12, sinv_y + 6, "INVENTORY", COLOR_GOLD_COIN, COLOR_BOX_DARK);
            draw_string_shadow(fb, sinv_x + 8, sinv_y + 20, "[SELL ITEMS]", COLOR_TEXT_WHITE, COLOR_BOX_DARK);

            // Bottom Controls: REROLL and NEXT BLIND
            bool can_reroll = (game->dollars >= game->shop.reroll_cost);
            draw_rounded_panel(fb, 8, 140, 110, 44, can_reroll ? COLOR_RGB(24, 14, 4) : COLOR_RGB(8, 8, 8), can_reroll ? COLOR_GOLD_COIN : COLOR_BOX_BORDER);
            char r_str[24];
            sprintf(r_str, "REROLL ($%d)", game->shop.reroll_cost);
            draw_string_shadow(fb, 18, 156, r_str, can_reroll ? COLOR_GOLD_COIN : COLOR_RGB(16, 16, 16), COLOR_BOX_DARK);

            draw_rounded_panel(fb, 124, 140, 124, 44, COLOR_RGB(4, 26, 10), COLOR_GOLD_COIN);
            draw_string_2x_shadow(fb, 134, 152, "NEXT BLIND", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
            return;
        } else {
            // --- Shop Inventory Sell Mode ---
            // Top Bar
            draw_rounded_panel(fb, 6, 4, 96, 20, COLOR_BOX_DARK, COLOR_GOLD_COIN);
            draw_string(fb, 10, 9, "< BACK TO SHOP", COLOR_GOLD_COIN);

            draw_rounded_panel(fb, 108, 4, 86, 20, COLOR_BOX_DARK, COLOR_BOX_BORDER);
            draw_string_shadow(fb, 116, 9, "SELL ITEMS", COLOR_TEXT_WHITE, COLOR_BOX_DARK);

            draw_rounded_panel(fb, 200, 4, 50, 20, COLOR_RGB(24, 18, 4), COLOR_GOLD_COIN);
            char cash_str[16];
            sprintf(cash_str, "$%d", game->dollars);
            draw_string_shadow(fb, 208, 9, cash_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

            // Owned Jokers (5 slots)
            draw_string(fb, 8, 28, "TAP A JOKER TO SELL:", COLOR_RGB(18, 24, 18));
            for (int j = 0; j < 5; ++j) {
                int jx = 4 + j * 50;
                int jy = 38;
                draw_rounded_panel(fb, jx, jy, 48, 52, COLOR_BOX_DARK, COLOR_BOX_BORDER);

                if (j < game->joker_count && game->jokers[j]) {
                    renderer_draw_joker_sprite(fb, jx + 11, jy + 2, game->jokers[j], false);
                    draw_rounded_panel(fb, jx + 3, jy + 36, 42, 13, COLOR_RGB(26, 4, 6), COLOR_MULT_RED);
                    char s_val[20];
                    sprintf(s_val, "SELL $%d", (int)game->jokers[j]->sell_value);
                    draw_string(fb, jx + 5, jy + 39, s_val, COLOR_TEXT_WHITE);
                } else {
                    draw_string(fb, jx + 18, jy + 20, "-", COLOR_RGB(10, 16, 12));
                }
            }

            // Owned Consumables (2 slots)
            draw_string(fb, 8, 96, "TAP A CONSUMABLE TO SELL:", COLOR_RGB(18, 24, 18));
            for (int c = 0; c < 2; ++c) {
                int cx = 8 + c * 124;
                int cy = 106;
                draw_rounded_panel(fb, cx, cy, 116, 42, COLOR_BOX_DARK, COLOR_BOX_BORDER);

                if (c < game->consumable_count && game->consumables[c].type != CONSUMABLE_NONE) {
                    renderer_draw_consumable_sprite(fb, cx + 4, cy + 3, &game->consumables[c], false);
                    draw_string(fb, cx + 34, cy + 6, game->consumables[c].name, COLOR_GOLD_COIN);

                    draw_rounded_panel(fb, cx + 34, cy + 20, 68, 16, COLOR_RGB(26, 4, 6), COLOR_MULT_RED);
                    draw_string(fb, cx + 40, cy + 24, "SELL ($1)", COLOR_TEXT_WHITE);
                } else {
                    draw_string(fb, cx + 38, cy + 16, "(EMPTY)", COLOR_RGB(12, 16, 14));
                }
            }

            // Return to Shop Button
            draw_rounded_panel(fb, 24, 154, 208, 32, COLOR_RGB(4, 26, 10), COLOR_GOLD_COIN);
            draw_string_shadow(fb, 68, 164, "DONE - RETURN TO SHOP", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
            return;
        }
    }

    // --- State: Booster Opening ---
    if (game->current_state == STATE_BOOSTER_OPEN) {
        draw_rounded_panel(fb, 14, 8, 228, 24, COLOR_BOX_DARK, COLOR_BOX_BORDER);
        draw_string_shadow(fb, 48, 14, "CHOOSE 1 CARD TO ADD TO DECK", COLOR_GOLD_COIN, COLOR_BOX_DARK);

        for (int i = 0; i < 3; ++i) {
            int cx = 20 + i * 76;
            int cy = 46;
            const ShopSlot* card_slot = &game->shop.pack_cards[i];
            draw_rounded_panel(fb, cx, cy, 66, 88, COLOR_BOX_DARK, COLOR_BOX_BORDER);

            if (card_slot->type == SHOP_ITEM_TAROT || card_slot->type == SHOP_ITEM_PLANET) {
                renderer_draw_consumable_sprite(fb, cx + 20, cy + 8, &card_slot->data.consumable, false);
                draw_string(fb, cx + 4, cy + 48, card_slot->data.consumable.name, COLOR_TEXT_WHITE);
            } else if (card_slot->type == SHOP_ITEM_JOKER) {
                renderer_draw_joker_sprite(fb, cx + 20, cy + 8, &card_slot->data.joker, false);
                draw_string(fb, cx + 4, cy + 48, card_slot->data.joker.name, COLOR_TEXT_WHITE);
            }

            draw_rounded_panel(fb, cx + 8, cy + 66, 50, 16, COLOR_RGB(4, 20, 8), COLOR_GOLD_COIN);
            draw_string(fb, cx + 14, cy + 70, "SELECT", COLOR_GOLD_COIN);
        }

        draw_rounded_panel(fb, 76, 148, 104, 34, COLOR_RGB(16, 4, 6), COLOR_MULT_RED);
        draw_string_shadow(fb, 100, 158, "SKIP PACK", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // --- State: Active Round & Scoring Controls ---
    // 1. Top Controls Bar (Sort buttons, Consumable Slots, Use/Sell buttons)
    draw_rounded_panel(fb, 4, 4, 54, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 10, 9, "SORT:RNK", COLOR_TEXT_WHITE);

    draw_rounded_panel(fb, 62, 4, 54, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 68, 9, "SORT:SUT", COLOR_TEXT_WHITE);

    // Consumable Slot 0 at (122, 2)
    draw_rounded_panel(fb, 122, 2, 28, 38, COLOR_BOX_DARK, (game->selected_consumable_slot == 0) ? COLOR_GOLD_COIN : COLOR_BOX_BORDER);
    if (game->consumables[0].type != CONSUMABLE_NONE) {
        renderer_draw_consumable_sprite(fb, 123, 3, &game->consumables[0], game->selected_consumable_slot == 0);
    } else {
        draw_string(fb, 131, 16, "T", COLOR_RGB(10, 18, 12));
    }

    // Consumable Slot 1 at (154, 2)
    draw_rounded_panel(fb, 154, 2, 28, 38, COLOR_BOX_DARK, (game->selected_consumable_slot == 1) ? COLOR_GOLD_COIN : COLOR_BOX_BORDER);
    if (game->consumables[1].type != CONSUMABLE_NONE) {
        renderer_draw_consumable_sprite(fb, 155, 3, &game->consumables[1], game->selected_consumable_slot == 1);
    } else {
        draw_string(fb, 163, 16, "T", COLOR_RGB(10, 18, 12));
    }

    // Action buttons or Stats on Right
    int sel_consumable = game->selected_consumable_slot;
    const Card* sel_cards[5];
    int sel_count = deck_get_selected_cards((DeckManager*)&game->deck, sel_cards);

    if (sel_consumable >= 0 && sel_consumable < MAX_CONSUMABLES_SLOTS && game->consumables[sel_consumable].type != CONSUMABLE_NONE) {
        Consumable* sc = (Consumable*)&game->consumables[sel_consumable];
        bool can_use = consumable_can_use(sc, sel_count);

        // Info below sort buttons
        draw_string(fb, 4, 25, sc->name, COLOR_GOLD_COIN);
        if (sc->min_selected > 0) {
            char req_str[24];
            sprintf(req_str, "Req: %d-%d cards", sc->min_selected, sc->max_selected);
            draw_string(fb, 4, 34, req_str, can_use ? COLOR_TEXT_WHITE : COLOR_DIAMOND_ORANGE);
        } else {
            draw_string(fb, 4, 34, "Instant use", COLOR_TEXT_WHITE);
        }

        // [USE CARD] Button at (186, 2)
        uint16_t use_bg = can_use ? COLOR_RGB(4, 24, 8) : COLOR_RGB(8, 8, 8);
        uint16_t use_bd = can_use ? COLOR_GOLD_COIN : COLOR_BOX_BORDER;
        draw_rounded_panel(fb, 186, 2, 66, 18, use_bg, use_bd);
        draw_string(fb, 192, 7, can_use ? "USE CARD" : "NEED SEL", can_use ? COLOR_TEXT_WHITE : COLOR_RGB(16, 16, 16));

        // [SELL $1] Button at (186, 22)
        draw_rounded_panel(fb, 186, 22, 66, 18, COLOR_RGB(24, 4, 6), COLOR_MULT_RED);
        draw_string(fb, 196, 27, "SELL $1", COLOR_TEXT_WHITE);
    } else {
        draw_string(fb, 6, 26, "SELECT 1-5 CARDS", COLOR_RGB(14, 22, 16));

        char j_badge[16];
        sprintf(j_badge, "JOKERS: %d/5", game->joker_count);
        draw_string(fb, 186, 7, j_badge, COLOR_TEXT_WHITE);

        char d_badge[16];
        sprintf(d_badge, "DECK: %d", game->deck.draw_pile.count);
        draw_string(fb, 186, 23, d_badge, COLOR_GOLD_COIN);
    }

    // 2. Playing Cards in Hand (Centered & 28x40 Crisp Graphics)
    int count = game->deck.hand.count;
    int total_hand_w = count * 31 - 3;
    int start_x = (DS_SCREEN_W - total_hand_w) / 2;
    if (start_x < 2) start_x = 2;
    int base_y = 74;

    for (int i = 0; i < count; ++i) {
        int cx = start_x + (i * 31);
        int cy = base_y;

        // Lift card if selected, or if scoring in animation
        if (game->current_state == STATE_ROUND_SCORING) {
            if (i == (game->scoring_step - 1)) cy = base_y - 20; // Active scoring card pops up!
            else if (game->deck.hand.cards[i].is_selected) cy = base_y - 14;
        } else if (game->deck.hand.cards[i].is_selected) {
            cy = base_y - 14;
        }

        bool debuffed = (game->current_blind == BLIND_BOSS) &&
                        boss_blind_is_card_debuffed(game->current_boss, &game->deck.hand.cards[i]);

        renderer_draw_card_sprite(fb, cx, cy, &game->deck.hand.cards[i], debuffed);

        // Under-card Enhancement / Index Badge
        const Card* c = &game->deck.hand.cards[i];
        if (c->enhancement == ENHANCEMENT_BONUS) draw_string(fb, cx + 2, base_y + 42, "+30C", COLOR_CHIPS_BLUE);
        else if (c->enhancement == ENHANCEMENT_MULT) draw_string(fb, cx + 4, base_y + 42, "+4M", COLOR_MULT_RED);
        else if (c->enhancement == ENHANCEMENT_WILD) draw_string(fb, cx + 2, base_y + 42, "WILD", COLOR_GOLD_COIN);
        else if (c->enhancement == ENHANCEMENT_GLASS) draw_string(fb, cx + 2, base_y + 42, "GLAS", COLOR_RGB(12, 28, 31));
        else if (c->enhancement == ENHANCEMENT_STEEL) draw_string(fb, cx + 1, base_y + 42, "STEL", COLOR_RGB(24, 24, 24));
        else if (c->enhancement == ENHANCEMENT_GOLD) draw_string(fb, cx + 2, base_y + 42, "GOLD", COLOR_GOLD_COIN);
        else if (c->enhancement == ENHANCEMENT_LUCKY) draw_string(fb, cx + 2, base_y + 42, "LUCK", COLOR_RGB(6, 28, 12));
        else if (c->enhancement == ENHANCEMENT_STONE) draw_string(fb, cx + 1, base_y + 42, "STON", COLOR_RGB(16, 16, 16));
        else {
            char num_buf[4];
            sprintf(num_buf, "%d", i + 1);
            uint16_t num_col = c->is_selected ? COLOR_CARD_SELECTED : COLOR_RGB(10, 18, 14);
            draw_string(fb, cx + 11, base_y + 42, num_buf, num_col);
        }
    }

    // 3. Bottom Action Buttons
    if (game->current_state == STATE_ROUND_SCORING) {
        draw_rounded_panel(fb, 8, 136, 240, 48, COLOR_RGB(4, 18, 28), COLOR_GOLD_COIN);
        draw_outline_rect(fb, 9, 137, 238, 46, COLOR_GOLD_COIN);
        draw_string_2x_shadow(fb, 46, 144, "SCORING HAND...", COLOR_GOLD_COIN, COLOR_BOX_DARK);
        draw_string_shadow(fb, 62, 166, "[TAP SCREEN TO SKIP >>]", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        return;
    }

    // PLAY HAND Button at (8, 136)
    uint16_t play_bg = (sel_count > 0) ? COLOR_PLAY_BTN : COLOR_RGB(2, 8, 14);
    draw_rounded_panel(fb, 8, 136, 116, 48, play_bg, COLOR_RGB(10, 24, 31));
    draw_outline_rect(fb, 9, 137, 114, 46, (sel_count > 0) ? COLOR_CHIPS_BLUE : COLOR_RGB(4, 12, 18));
    if (sel_count > 0) {
        draw_string_2x_shadow(fb, 36, 142, "PLAY", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        char est_pts[24];
        int64_t est_total = (int64_t)(game->preview_score.chips * game->preview_score.mult);
        sprintf(est_pts, "%lld PTS", (long long)est_total);
        draw_string_shadow(fb, 26, 166, est_pts, COLOR_GOLD_COIN, COLOR_BOX_DARK);
    } else {
        draw_string(fb, 18, 154, "SELECT CARDS", COLOR_RGB(12, 16, 18));
    }

    // DISCARD Button at (132, 136)
    bool can_discard = (game->discards_left > 0 && sel_count > 0);
    uint16_t disc_bg = can_discard ? COLOR_DISCARD_BTN : COLOR_RGB(14, 3, 4);
    draw_rounded_panel(fb, 132, 136, 116, 48, disc_bg, COLOR_RGB(31, 10, 12));
    draw_outline_rect(fb, 133, 137, 114, 46, can_discard ? COLOR_MULT_RED : COLOR_RGB(18, 4, 6));
    if (game->discards_left <= 0) {
        draw_string(fb, 146, 154, "NO DISCARDS", COLOR_RGB(18, 10, 10));
    } else if (sel_count > 0) {
        draw_string_2x_shadow(fb, 144, 142, "DISCARD", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
        char disc_left[24];
        sprintf(disc_left, "(%d LEFT)", game->discards_left);
        draw_string_shadow(fb, 164, 166, disc_left, COLOR_TEXT_WHITE, COLOR_BOX_DARK);
    } else {
        draw_string(fb, 160, 154, "DISCARD", COLOR_RGB(16, 12, 12));
    }
}

