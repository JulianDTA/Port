#include "platform/renderer_ds.h"
#include <stdio.h>
#include <string.h>

// 5x7 ASCII bitmap font (characters 32 ' ' to 126 '~')
// Each byte represents a column of 7 vertical pixels (bit 0 = top, bit 6 = bottom)
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

// 7x7 Suit Pixel Icons (1 = colored pixel, 0 = transparent)
static const uint8_t s_suit_heart[7] = {
    0b0110110,
    0b1111111,
    0b1111111,
    0b1111111,
    0b0111110,
    0b0011100,
    0b0001000
};

static const uint8_t s_suit_diamond[7] = {
    0b0001000,
    0b0011100,
    0b0111110,
    0b1111111,
    0b0111110,
    0b0011100,
    0b0001000
};

static const uint8_t s_suit_spade[7] = {
    0b0001000,
    0b0011100,
    0b0111110,
    0b1111111,
    0b1101011,
    0b0001000,
    0b0011100
};

static const uint8_t s_suit_club[7] = {
    0b0011100,
    0b0011100,
    0b1101011,
    0b1111111,
    0b0101010,
    0b0001000,
    0b0011100
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
    // Fill interior
    draw_fill_rect(fb, x + 1, y + 1, w - 2, h - 2, fill_col);
    // Borders
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
            cur_x += 6; // 5 pixels char + 1 pixel spacing
        }
        str++;
    }
}

void draw_string_shadow(uint16_t* fb, int x, int y, const char* str, uint16_t text_col, uint16_t shadow_col) {
    draw_string(fb, x + 1, y + 1, str, shadow_col);
    draw_string(fb, x, y, str, text_col);
}

// Draw 7x7 suit pip
static void draw_suit_icon(uint16_t* fb, int x, int y, CardSuit suit) {
    const uint8_t* pattern = NULL;
    uint16_t col = COLOR_HEART_RED;

    switch (suit) {
        case SUIT_HEARTS:
            pattern = s_suit_heart;
            col = COLOR_HEART_RED;
            break;
        case SUIT_DIAMONDS:
            pattern = s_suit_diamond;
            col = COLOR_DIAMOND_ORANGE;
            break;
        case SUIT_SPADES:
            pattern = s_suit_spade;
            col = COLOR_SPADE_BLACK;
            break;
        case SUIT_CLUBS:
            pattern = s_suit_club;
            col = COLOR_CLUB_BLACK;
            break;
        default:
            return;
    }

    for (int r = 0; r < 7; ++r) {
        int py = y + r;
        if (py < 0 || py >= DS_SCREEN_H) continue;
        uint8_t row_bits = pattern[r];
        for (int c = 0; c < 7; ++c) {
            int px = x + c;
            if (px < 0 || px >= DS_SCREEN_W) continue;
            if (row_bits & (1 << (6 - c))) {
                fb[py * DS_SCREEN_W + px] = col;
            }
        }
    }
}

// -------------------------------------------------------------
// Card Rendering (Balatro Graphic Playing Card)
// -------------------------------------------------------------

static void draw_playing_card(uint16_t* fb, int x, int y, const Card* card) {
    const int W = 26;
    const int H = 38;

    // Drop shadow
    draw_fill_rect(fb, x + 2, y + 2, W, H, COLOR_CARD_SHADOW);

    // Card background & border
    uint16_t border_col = card->is_selected ? COLOR_CARD_SELECTED : COLOR_BOX_BORDER;
    uint16_t bg_col = COLOR_CARD_WHITE;

    if (card->enhancement == ENHANCEMENT_STONE) {
        bg_col = COLOR_RGB(16, 16, 16); // Stone grey
    } else if (card->enhancement == ENHANCEMENT_STEEL) {
        bg_col = COLOR_RGB(22, 24, 26); // Steel metallic
    } else if (card->enhancement == ENHANCEMENT_GLASS) {
        bg_col = COLOR_RGB(25, 29, 31); // Glass tint
    }

    draw_rounded_panel(fb, x, y, W, H, bg_col, border_col);

    // Extra highlight border if selected
    if (card->is_selected) {
        draw_outline_rect(fb, x - 1, y - 1, W + 2, H + 2, COLOR_CARD_SELECTED);
    }

    if (card->enhancement == ENHANCEMENT_STONE) {
        draw_string(fb, x + 3, y + 15, "+50", COLOR_TEXT_WHITE);
        return;
    }

    // Rank text color (red for Hearts/Diamonds, dark for Spades/Clubs)
    uint16_t font_col = (card->suit == SUIT_HEARTS || card->suit == SUIT_DIAMONDS)
                        ? COLOR_HEART_RED : COLOR_SPADE_BLACK;

    const char* r_str = card_rank_name(card->rank);
    char buf[4];
    if (card->rank == RANK_10) {
        strcpy(buf, "10");
    } else {
        buf[0] = r_str[0];
        buf[1] = '\0';
    }

    // Top-left rank
    draw_string(fb, x + 2, y + 2, buf, font_col);

    // Top-right mini suit
    draw_suit_icon(fb, x + 16, y + 2, card->suit);

    // Center big suit pip
    draw_suit_icon(fb, x + 9, y + 16, card->suit);

    // Bottom-right rank
    draw_string(fb, x + 18, y + 28, buf, font_col);
}

// -------------------------------------------------------------
// Top Screen Rendering (Status, Jokers, Blind Target, Chips x Mult)
// -------------------------------------------------------------

void renderer_draw_top_screen(uint16_t* fb, const GameSession* game) {
    if (!fb || !game) return;

    // Fill table background
    draw_fill_rect(fb, 0, 0, DS_SCREEN_W, DS_SCREEN_H, COLOR_TABLE_BG);

    // Subtle table felt border
    draw_outline_rect(fb, 2, 2, DS_SCREEN_W - 4, DS_SCREEN_H - 4, COLOR_TABLE_ACCENT);

    // Top Title
    draw_rounded_panel(fb, 80, 4, 96, 14, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string_shadow(fb, 94, 7, "* BALATRO *", COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // Ante & Blind Badges (Left top)
    draw_rounded_panel(fb, 6, 22, 60, 16, COLOR_RGB(28, 12, 4), COLOR_DIAMOND_ORANGE);
    char ante_str[16];
    sprintf(ante_str, "ANTE %d/8", game->ante);
    draw_string(fb, 10, 26, ante_str, COLOR_TEXT_WHITE);

    draw_rounded_panel(fb, 70, 22, 78, 16, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    const char* blind_name = (game->current_blind == BLIND_SMALL ? "SMALL BLIND" :
                             (game->current_blind == BLIND_BIG   ? "BIG BLIND"   : "BOSS BLIND"));
    draw_string(fb, 74, 26, blind_name, COLOR_GOLD_COIN);

    // Target Score Panel (Left middle)
    draw_rounded_panel(fb, 6, 42, 142, 28, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 10, 45, "SCORE AT LEAST:", COLOR_RGB(18, 22, 20));
    char goal_str[32];
    sprintf(goal_str, "%lld PTS", (long long)game->target_score);
    draw_string_shadow(fb, 10, 56, goal_str, COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // Round Progress / Current Score
    draw_rounded_panel(fb, 6, 73, 142, 24, COLOR_RGB(3, 7, 5), COLOR_BOX_BORDER);
    draw_string(fb, 10, 76, "ROUND SCORE:", COLOR_RGB(16, 20, 18));
    char score_str[32];
    sprintf(score_str, "%lld", (long long)game->current_round_score);
    draw_string_shadow(fb, 80, 76, score_str, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Jokers Tray (Right side: 5 slots)
    draw_rounded_panel(fb, 152, 22, 98, 75, COLOR_BOX_DARK, COLOR_BOX_BORDER);
    draw_string(fb, 160, 25, "JOKERS (0-5)", COLOR_RGB(16, 22, 18));

    for (int i = 0; i < MAX_JOKERS_SLOTS; ++i) {
        int jx = 156 + (i % 3) * 30;
        int jy = 36 + (i / 3) * 28;
        if (i < game->joker_count && game->jokers[i]) {
            // Draw active joker mini card
            draw_rounded_panel(fb, jx, jy, 26, 24, COLOR_RGB(28, 24, 18), COLOR_GOLD_COIN);
            draw_string(fb, jx + 4, jy + 4, "JK", COLOR_MULT_RED);
            char short_name[4];
            strncpy(short_name, game->jokers[i]->name, 3);
            short_name[3] = '\0';
            draw_string(fb, jx + 3, jy + 14, short_name, COLOR_TEXT_BLACK);
        } else {
            // Empty dashed slot
            draw_outline_rect(fb, jx, jy, 26, 24, COLOR_RGB(6, 12, 8));
        }
    }

    // Iconic Chips & Mult Calculator Box (Balatro Signature)
    draw_rounded_panel(fb, 6, 102, 244, 46, COLOR_BOX_DARK, COLOR_BOX_BORDER);

    // Blue Chips Box
    draw_rounded_panel(fb, 12, 108, 90, 34, COLOR_CHIPS_BLUE, COLOR_RGB(8, 22, 31));
    draw_string(fb, 18, 112, "CHIPS", COLOR_RGB(18, 24, 31));
    char chips_val[16];
    sprintf(chips_val, "%lld", (long long)game->last_score.chips);
    draw_string_shadow(fb, 18, 124, chips_val, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Center X multiplication sign
    draw_string_shadow(fb, 108, 120, "X", COLOR_GOLD_COIN, COLOR_BOX_DARK);

    // Red Mult Box
    draw_rounded_panel(fb, 122, 108, 90, 34, COLOR_MULT_RED, COLOR_RGB(31, 10, 12));
    draw_string(fb, 128, 112, "MULT", COLOR_RGB(31, 20, 22));
    char mult_val[16];
    sprintf(mult_val, "%.1f", game->last_score.mult);
    draw_string_shadow(fb, 128, 124, mult_val, COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // Hand Played Name Badge
    if (game->last_eval.type != HAND_NONE) {
        const char* hname = poker_hand_name(game->last_eval.type);
        draw_string_shadow(fb, 10, 153, hname, COLOR_GOLD_COIN, COLOR_BOX_DARK);
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
    draw_outline_rect(fb, 2, 2, DS_SCREEN_W - 4, DS_SCREEN_H - 4, COLOR_TABLE_ACCENT);

    // Header label
    draw_string(fb, 48, 10, "--- TAP CARDS TO SELECT ---", COLOR_RGB(12, 22, 16));

    // Draw the 8 Cards in Hand spread out horizontally
    // Screen width = 256. Card width = 26.
    // 8 cards spaced by 30 px = 240 px total span. Start at x = 8.
    int start_x = 8;
    int base_y = 66;

    for (int i = 0; i < game->deck.hand.count; ++i) {
        int cx = start_x + (i * 30);
        // If selected, card LIFTS UP by 12 pixels just like Balatro!
        int cy = game->deck.hand.cards[i].is_selected ? (base_y - 12) : base_y;

        draw_playing_card(fb, cx, cy, &game->deck.hand.cards[i]);

        // Draw card index number below
        char num_buf[4];
        sprintf(num_buf, "%d", i + 1);
        draw_string(fb, cx + 10, base_y + 42, num_buf, COLOR_RGB(10, 18, 14));
    }

    // Touch Action Buttons (Bottom area)
    // PLAY HAND Button (Big Blue)
    draw_rounded_panel(fb, 16, 138, 108, 42, COLOR_PLAY_BTN, COLOR_RGB(10, 24, 31));
    draw_outline_rect(fb, 17, 139, 106, 40, COLOR_CHIPS_BLUE);
    draw_string_shadow(fb, 30, 154, "PLAY HAND", COLOR_TEXT_WHITE, COLOR_BOX_DARK);

    // DISCARD Button (Big Red)
    draw_rounded_panel(fb, 132, 138, 108, 42, COLOR_DISCARD_BTN, COLOR_RGB(31, 10, 12));
    draw_outline_rect(fb, 133, 139, 106, 40, COLOR_MULT_RED);
    draw_string_shadow(fb, 154, 154, "DISCARD", COLOR_TEXT_WHITE, COLOR_BOX_DARK);
}
