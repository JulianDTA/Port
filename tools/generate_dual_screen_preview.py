import os
import re
from PIL import Image, ImageDraw

DS_W = 256
DS_H = 192

# Font 5x7 ASCII bitmap table from renderer_ds.c
FONT_5X7 = [
    [0x00,0x00,0x00,0x00,0x00], # 32 ' '
    [0x00,0x00,0x5F,0x00,0x00], # 33 '!'
    [0x00,0x07,0x00,0x07,0x00], # 34 '"'
    [0x14,0x7F,0x14,0x7F,0x14], # 35 '#'
    [0x24,0x2A,0x7F,0x2A,0x12], # 36 '$'
    [0x23,0x13,0x08,0x64,0x62], # 37 '%'
    [0x36,0x49,0x55,0x22,0x50], # 38 '&'
    [0x00,0x05,0x03,0x00,0x00], # 39 '''
    [0x00,0x1C,0x22,0x41,0x00], # 40 '('
    [0x00,0x41,0x22,0x1C,0x00], # 41 ')'
    [0x14,0x08,0x3E,0x08,0x14], # 42 '*'
    [0x08,0x08,0x3E,0x08,0x08], # 43 '+'
    [0x00,0x50,0x30,0x00,0x00], # 44 ','
    [0x08,0x08,0x08,0x08,0x08], # 45 '-'
    [0x00,0x60,0x60,0x00,0x00], # 46 '.'
    [0x20,0x10,0x08,0x04,0x02], # 47 '/'
    [0x3E,0x51,0x49,0x45,0x3E], # 48 '0'
    [0x00,0x42,0x7F,0x40,0x00], # 49 '1'
    [0x42,0x61,0x51,0x49,0x46], # 50 '2'
    [0x21,0x41,0x45,0x4B,0x31], # 51 '3'
    [0x18,0x14,0x12,0x7F,0x10], # 52 '4'
    [0x27,0x45,0x45,0x45,0x39], # 53 '5'
    [0x3C,0x4A,0x49,0x49,0x30], # 54 '6'
    [0x01,0x71,0x09,0x05,0x03], # 55 '7'
    [0x36,0x49,0x49,0x49,0x36], # 56 '8'
    [0x06,0x49,0x49,0x29,0x1E], # 57 '9'
    [0x00,0x36,0x36,0x00,0x00], # 58 ':'
    [0x00,0x56,0x36,0x00,0x00], # 59 ';'
    [0x08,0x14,0x22,0x41,0x00], # 60 '<'
    [0x14,0x14,0x14,0x14,0x14], # 61 '='
    [0x00,0x41,0x22,0x14,0x08], # 62 '>'
    [0x02,0x01,0x51,0x09,0x06], # 63 '?'
    [0x32,0x49,0x79,0x41,0x3E], # 64 '@'
    [0x7E,0x11,0x11,0x11,0x7E], # 65 'A'
    [0x7F,0x49,0x49,0x49,0x36], # 66 'B'
    [0x3E,0x41,0x41,0x41,0x22], # 67 'C'
    [0x7F,0x41,0x41,0x22,0x1C], # 68 'D'
    [0x7F,0x49,0x49,0x49,0x41], # 69 'E'
    [0x7F,0x09,0x09,0x09,0x01], # 70 'F'
    [0x3E,0x41,0x49,0x49,0x7A], # 71 'G'
    [0x7F,0x08,0x08,0x08,0x7F], # 72 'H'
    [0x00,0x41,0x7F,0x41,0x00], # 73 'I'
    [0x20,0x40,0x41,0x3F,0x01], # 74 'J'
    [0x7F,0x08,0x14,0x22,0x41], # 75 'K'
    [0x7F,0x40,0x40,0x40,0x40], # 76 'L'
    [0x7F,0x02,0x0C,0x02,0x7F], # 77 'M'
    [0x7F,0x04,0x08,0x10,0x7F], # 78 'N'
    [0x3E,0x41,0x41,0x41,0x3E], # 79 'O'
    [0x7F,0x09,0x09,0x09,0x06], # 80 'P'
    [0x3E,0x41,0x51,0x21,0x5E], # 81 'Q'
    [0x7F,0x09,0x19,0x29,0x46], # 82 'R'
    [0x46,0x49,0x49,0x49,0x31], # 83 'S'
    [0x01,0x01,0x7F,0x01,0x01], # 84 'T'
    [0x3F,0x40,0x40,0x40,0x3F], # 85 'U'
    [0x1F,0x20,0x40,0x20,0x1F], # 86 'V'
    [0x3F,0x40,0x38,0x40,0x3F], # 87 'W'
    [0x63,0x14,0x08,0x14,0x63], # 88 'X'
    [0x07,0x08,0x70,0x08,0x07], # 89 'Y'
    [0x61,0x51,0x49,0x45,0x43], # 90 'Z'
    [0x00,0x7F,0x41,0x41,0x00], # 91 '['
    [0x02,0x04,0x08,0x10,0x20], # 92 '\'
    [0x00,0x41,0x41,0x7F,0x00], # 93 ']'
    [0x04,0x02,0x01,0x02,0x04], # 94 '^'
    [0x40,0x40,0x40,0x40,0x40], # 95 '_'
    [0x00,0x01,0x02,0x04,0x00], # 96 '`'
    [0x20,0x54,0x54,0x54,0x78], # 97 'a'
    [0x7F,0x48,0x44,0x44,0x38], # 98 'b'
    [0x38,0x44,0x44,0x44,0x20], # 99 'c'
    [0x38,0x44,0x44,0x48,0x7F], # 100 'd'
    [0x38,0x54,0x54,0x54,0x18], # 101 'e'
    [0x08,0x7E,0x09,0x01,0x02], # 102 'f'
    [0x0C,0x52,0x52,0x52,0x3E], # 103 'g'
    [0x7F,0x08,0x04,0x04,0x78], # 104 'h'
    [0x00,0x44,0x7D,0x40,0x00], # 105 'i'
    [0x20,0x40,0x44,0x3D,0x00], # 106 'j'
    [0x7F,0x10,0x28,0x44,0x00], # 107 'k'
    [0x00,0x41,0x7F,0x40,0x00], # 108 'l'
    [0x7C,0x04,0x18,0x04,0x78], # 109 'm'
    [0x7C,0x08,0x04,0x04,0x78], # 110 'n'
    [0x38,0x44,0x44,0x44,0x38], # 111 'o'
    [0x7C,0x14,0x14,0x14,0x08], # 112 'p'
    [0x08,0x14,0x14,0x18,0x7C], # 113 'q'
    [0x7C,0x08,0x04,0x04,0x08], # 114 'r'
    [0x48,0x54,0x54,0x54,0x20], # 115 's'
    [0x04,0x3F,0x44,0x40,0x20], # 116 't'
    [0x3C,0x40,0x40,0x20,0x7C], # 117 'u'
    [0x1C,0x20,0x40,0x20,0x1C], # 118 'v'
    [0x3C,0x40,0x30,0x40,0x3C], # 119 'w'
    [0x44,0x28,0x10,0x28,0x44], # 120 'x'
    [0x0C,0x50,0x50,0x50,0x3C], # 121 'y'
    [0x44,0x64,0x54,0x4C,0x44], # 122 'z'
    [0x00,0x08,0x36,0x41,0x00], # 123 '{'
    [0x00,0x00,0x7F,0x00,0x00], # 124 '|'
    [0x00,0x41,0x36,0x08,0x00], # 125 '}'
    [0x08,0x04,0x08,0x10,0x08]  # 126 '~'
]

def col_rgb(r, g, b):
    # 5-bit RGB to 8-bit
    return (int((r & 0x1F) * 255 / 31), int((g & 0x1F) * 255 / 31), int((b & 0x1F) * 255 / 31))

COLOR_TABLE_BG       = col_rgb(4, 9, 7)
COLOR_TABLE_ACCENT   = col_rgb(6, 13, 10)
COLOR_CARD_SHADOW    = col_rgb(2, 5, 3)
COLOR_CARD_SELECTED  = col_rgb(31, 28, 5)
COLOR_CHIPS_BLUE     = col_rgb(3, 14, 28)
COLOR_MULT_RED       = col_rgb(28, 5, 6)
COLOR_GOLD_COIN      = col_rgb(31, 25, 4)
COLOR_TEXT_WHITE     = col_rgb(31, 31, 31)
COLOR_BOX_DARK       = col_rgb(2, 4, 3)
COLOR_BOX_BORDER     = col_rgb(8, 16, 12)
COLOR_DIAMOND_ORANGE = col_rgb(30, 13, 3)
COLOR_PLAY_BTN       = col_rgb(4, 16, 28)
COLOR_DISCARD_BTN    = col_rgb(26, 4, 6)

def val15_to_rgb(v):
    r = (v & 0x1F) * 255 // 31
    g = ((v >> 5) & 0x1F) * 255 // 31
    b = ((v >> 10) & 0x1F) * 255 // 31
    return (r, g, b)

# Parse raw hex arrays from header files
def parse_c_array(header_path, array_name):
    with open(header_path, 'r', encoding='utf-8') as f:
        content = f.read()
    pattern = rf'{array_name}\s*\[\s*\d+\s*\]\s*\[\s*\d+\s*\]\s*=\s*\{{(.*?)\}};'
    m = re.search(pattern, content, re.DOTALL)
    if not m:
        # Check single dimension array
        pattern2 = rf'{array_name}\s*\[\s*\d+\s*\]\s*=\s*\{{(.*?)\}};'
        m = re.search(pattern2, content, re.DOTALL)
    if not m:
        return []
    raw = m.group(1)
    # Extract blocks or hex numbers
    entries = []
    # If nested braces:
    if '{' in raw:
        sub_blocks = re.findall(r'\{(.*?)\}', raw, re.DOTALL)
        for sb in sub_blocks:
            vals = [int(x, 16) for x in re.findall(r'0x[0-9a-fA-F]+', sb)]
            entries.append(vals)
    else:
        vals = [int(x, 16) for x in re.findall(r'0x[0-9a-fA-F]+', raw)]
        entries.append(vals)
    return entries

print("Loading header assets...")
cards_gfx = parse_c_array("include/gfx/balatro_deck_gfx.h", "s_balatro_cards")
jokers_gfx = parse_c_array("include/gfx/balatro_jokers_gfx.h", "s_balatro_jokers")
cons_gfx = parse_c_array("include/gfx/balatro_consumables_gfx.h", "s_balatro_consumables")
small_chip_gfx = parse_c_array("include/gfx/balatro_chips_gfx.h", "s_chip_small_blind")[0]

print(f"Loaded {len(cards_gfx)} cards, {len(jokers_gfx)} jokers, {len(cons_gfx)} consumables, chip len {len(small_chip_gfx)}.")

class ScreenCanvas:
    def __init__(self):
        self.img = Image.new("RGB", (DS_W, DS_H), COLOR_TABLE_BG)
        self.draw = ImageDraw.Draw(self.img)
        self.draw_outline_rect(1, 1, DS_W - 2, DS_H - 2, COLOR_TABLE_ACCENT)

    def draw_fill_rect(self, x, y, w, h, col):
        if w <= 0 or h <= 0: return
        self.draw.rectangle([x, y, x + w - 1, y + h - 1], fill=col)

    def draw_outline_rect(self, x, y, w, h, col):
        if w <= 0 or h <= 0: return
        self.draw.rectangle([x, y, x + w - 1, y + h - 1], outline=col)

    def draw_rounded_panel(self, x, y, w, h, fill_col, border_col):
        if w < 4 or h < 4: return
        self.draw_fill_rect(x + 1, y + 1, w - 2, h - 2, fill_col)
        self.draw_fill_rect(x + 1, y, w - 2, 1, border_col)
        self.draw_fill_rect(x + 1, y + h - 1, w - 2, 1, border_col)
        self.draw_fill_rect(x, y + 1, 1, h - 2, border_col)
        self.draw_fill_rect(x + w - 1, y + 1, 1, h - 2, border_col)

    def draw_char(self, x, y, c, col):
        code = ord(c)
        if code < 32 or code > 126: code = ord('?')
        idx = code - 32
        bitmap = FONT_5X7[idx]
        for col_i in range(5):
            px = x + col_i
            if 0 <= px < DS_W:
                line = bitmap[col_i]
                for row_i in range(7):
                    py = y + row_i
                    if 0 <= py < DS_H and (line & (1 << row_i)):
                        self.img.putpixel((px, py), col)

    def draw_string(self, x, y, s, col):
        cur_x = x
        for c in s:
            if c == '\n':
                cur_x = x
                y += 8
            else:
                self.draw_char(cur_x, y, c, col)
                cur_x += 6

    def draw_string_shadow(self, x, y, s, col, sh_col):
        self.draw_string(x + 1, y + 1, s, sh_col)
        self.draw_string(x, y, s, col)

    def draw_char_2x(self, x, y, c, col):
        code = ord(c)
        if code < 32 or code > 126: code = ord('?')
        idx = code - 32
        bitmap = FONT_5X7[idx]
        for col_i in range(5):
            px = x + col_i * 2
            line = bitmap[col_i]
            for row_i in range(7):
                py = y + row_i * 2
                if (line & (1 << row_i)):
                    for dx in range(2):
                        for dy in range(2):
                            if 0 <= px + dx < DS_W and 0 <= py + dy < DS_H:
                                self.img.putpixel((px + dx, py + dy), col)

    def draw_string_2x(self, x, y, s, col):
        cur_x = x
        for c in s:
            self.draw_char_2x(cur_x, y, c, col)
            cur_x += 12

    def draw_string_2x_shadow(self, x, y, s, col, sh_col):
        self.draw_string_2x(x + 1, y + 1, s, sh_col)
        self.draw_string_2x(x, y, s, col)

    def blit_sprite(self, x, y, w, h, data, debuffed=False, selected=False):
        # Drop shadow
        self.draw_fill_rect(x + 2, y + 2, w, h, COLOR_CARD_SHADOW)
        for py in range(h):
            sy = y + py
            if not (0 <= sy < DS_H): continue
            for px in range(w):
                sx = x + px
                if not (0 <= sx < DS_W): continue
                c_val = data[py * w + px]
                if c_val != 0x0000:
                    if debuffed and (px == py or px == (w - 1 - py)):
                        self.img.putpixel((sx, sy), COLOR_MULT_RED)
                    else:
                        self.img.putpixel((sx, sy), val15_to_rgb(c_val))
        if selected:
            self.draw_outline_rect(x - 1, y - 1, w + 2, h + 2, COLOR_CARD_SELECTED)
            self.draw_outline_rect(x - 2, y - 2, w + 4, h + 4, COLOR_CARD_SELECTED)

def render_top_screen_active():
    cv = ScreenCanvas()
    # 1. 5 Jokers tray
    for j in range(5):
        jx = 8 + j * 49
        jy = 2
        if j < 3: # 3 jokers owned: Joker, Greedy, Cavendish
            cv.blit_sprite(jx, jy, 26, 36, jokers_gfx[j * 4])
        else:
            cv.draw_rounded_panel(jx, jy, 26, 36, COLOR_BOX_DARK, COLOR_BOX_BORDER)
            cv.draw_string(jx + 8, jy + 14, "+", col_rgb(8, 14, 10))

    # 2. Chip sprite (34x34)
    for py in range(34):
        sy = 40 + py
        for px in range(34):
            sx = 6 + px
            c = small_chip_gfx[py * 34 + px]
            if c != 0x0000:
                cv.img.putpixel((sx, sy), val15_to_rgb(c))

    # Ante & Blind
    cv.draw_rounded_panel(44, 40, 52, 12, col_rgb(28, 12, 4), COLOR_DIAMOND_ORANGE)
    cv.draw_string(47, 42, "ANTE 1/8", COLOR_TEXT_WHITE)
    cv.draw_string_shadow(44, 54, "SMALL BLIND", COLOR_GOLD_COIN, COLOR_BOX_DARK)
    cv.draw_string(44, 66, "Reward: +$3", col_rgb(16, 24, 18))

    # Target score
    cv.draw_rounded_panel(144, 40, 106, 36, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.draw_string(150, 43, "SCORE AT LEAST:", col_rgb(16, 22, 18))
    cv.draw_string_2x_shadow(150, 53, "300", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    # Round score
    cv.draw_rounded_panel(8, 78, 240, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.draw_string(14, 83, "ROUND SCORE:", col_rgb(16, 20, 18))
    cv.draw_string_shadow(86, 83, "0 / 300", COLOR_TEXT_WHITE, COLOR_BOX_DARK)
    cv.draw_outline_rect(195, 80, 50, 14, COLOR_BOX_BORDER)

    # Calculator box
    cv.draw_rounded_panel(6, 98, 244, 68, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    # Chips panel
    cv.draw_rounded_panel(12, 102, 98, 40, COLOR_CHIPS_BLUE, col_rgb(8, 24, 31))
    cv.draw_outline_rect(13, 103, 96, 38, col_rgb(16, 26, 31))
    cv.draw_string(16, 105, "CHIPS", col_rgb(18, 26, 31))
    cv.draw_string_2x_shadow(18, 117, "65", COLOR_TEXT_WHITE, COLOR_BOX_DARK)

    cv.draw_string_2x_shadow(118, 114, "X", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    # Mult panel
    cv.draw_rounded_panel(146, 102, 98, 40, COLOR_MULT_RED, col_rgb(31, 10, 12))
    cv.draw_outline_rect(147, 103, 96, 38, col_rgb(31, 18, 20))
    cv.draw_string(150, 105, "MULT", col_rgb(31, 22, 24))
    cv.draw_string_2x_shadow(152, 117, "4.0", COLOR_TEXT_WHITE, COLOR_BOX_DARK)

    # Hand name & Est
    cv.draw_string_shadow(14, 148, "Flush", COLOR_GOLD_COIN, COLOR_BOX_DARK)
    cv.draw_string_shadow(140, 148, "EST: 260 PTS", COLOR_TEXT_WHITE, COLOR_BOX_DARK)

    # Bottom stats
    cv.draw_rounded_panel(6, 168, 74, 18, col_rgb(20, 16, 4), COLOR_GOLD_COIN)
    cv.draw_string_shadow(14, 173, "$5", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    cv.draw_rounded_panel(86, 168, 78, 18, COLOR_CHIPS_BLUE, COLOR_BOX_BORDER)
    cv.draw_string(92, 173, "4 HANDS", COLOR_TEXT_WHITE)

    cv.draw_rounded_panel(170, 168, 80, 18, COLOR_MULT_RED, COLOR_BOX_BORDER)
    cv.draw_string(174, 173, "3 DISCARDS", COLOR_TEXT_WHITE)

    return cv.img

def render_bottom_screen_active():
    cv = ScreenCanvas()
    # Sort buttons
    cv.draw_rounded_panel(4, 4, 54, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.draw_string(10, 9, "SORT:RNK", COLOR_TEXT_WHITE)
    cv.draw_rounded_panel(62, 4, 54, 18, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.draw_string(68, 9, "SORT:SUT", COLOR_TEXT_WHITE)

    # Consumables slots
    # Slot 0: The Fool (selected)
    cv.draw_rounded_panel(122, 2, 28, 38, COLOR_BOX_DARK, COLOR_GOLD_COIN)
    cv.blit_sprite(123, 3, 26, 36, cons_gfx[0], selected=True)

    # Slot 1: Mercury
    cv.draw_rounded_panel(154, 2, 28, 38, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.blit_sprite(155, 3, 26, 36, cons_gfx[22], selected=False)

    # Action buttons
    cv.draw_string(4, 25, "The Fool", COLOR_GOLD_COIN)
    cv.draw_string(4, 34, "Instant use", COLOR_TEXT_WHITE)

    # [USE CARD]
    cv.draw_rounded_panel(186, 2, 66, 18, col_rgb(4, 24, 8), COLOR_GOLD_COIN)
    cv.draw_string(192, 7, "USE CARD", COLOR_TEXT_WHITE)

    # [SELL $1]
    cv.draw_rounded_panel(186, 22, 66, 18, col_rgb(24, 4, 6), COLOR_MULT_RED)
    cv.draw_string(196, 27, "SELL $1", COLOR_TEXT_WHITE)

    # Hand cards (8 cards, centered)
    # 5 selected cards (Hearts: 2, 4, 7, 10, Ace)
    hand_cards_idx = [0, 2, 5, 8, 12, 14, 28, 41] # Hearts & other suits
    selected_mask =  [True, True, True, True, True, False, False, False]
    base_y = 74
    total_w = 8 * 31 - 3
    start_x = (DS_W - total_w) // 2

    for i in range(8):
        cx = start_x + i * 31
        is_sel = selected_mask[i]
        cy = (base_y - 14) if is_sel else base_y
        card_data = cards_gfx[hand_cards_idx[i]]
        cv.blit_sprite(cx, cy, 28, 40, card_data, selected=is_sel)

        # Card number
        num_col = COLOR_CARD_SELECTED if is_sel else col_rgb(10, 18, 14)
        cv.draw_string(cx + 11, base_y + 42, str(i + 1), num_col)

    # Play Hand button
    cv.draw_rounded_panel(8, 136, 116, 48, COLOR_PLAY_BTN, col_rgb(10, 24, 31))
    cv.draw_outline_rect(9, 137, 114, 46, COLOR_CHIPS_BLUE)
    cv.draw_string_2x_shadow(36, 142, "PLAY", COLOR_TEXT_WHITE, COLOR_BOX_DARK)
    cv.draw_string_shadow(26, 166, "260 PTS", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    # Discard button
    cv.draw_rounded_panel(132, 136, 116, 48, COLOR_DISCARD_BTN, col_rgb(31, 10, 12))
    cv.draw_outline_rect(133, 137, 114, 46, COLOR_MULT_RED)
    cv.draw_string_2x_shadow(144, 142, "DISCARD", COLOR_TEXT_WHITE, COLOR_BOX_DARK)
    cv.draw_string_shadow(164, 166, "(3 LEFT)", COLOR_TEXT_WHITE, COLOR_BOX_DARK)

    return cv.img

def combine_dual_screen(top_img, bottom_img):
    # DS Frame with hinge
    canvas = Image.new("RGB", (DS_W + 16, DS_H * 2 + 32), (20, 20, 24))
    d = ImageDraw.Draw(canvas)
    # Bezel borders
    d.rectangle([6, 6, DS_W + 9, DS_H + 9], outline=(45, 45, 52), width=2)
    d.rectangle([6, DS_H + 22, DS_W + 9, DS_H * 2 + 25], outline=(45, 45, 52), width=2)
    # Paste screens
    canvas.paste(top_img, (8, 8))
    canvas.paste(bottom_img, (8, DS_H + 24))
    return canvas

def render_top_screen_shop():
    cv = ScreenCanvas()
    cv.draw_rounded_panel(70, 2, 116, 16, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.draw_string_shadow(80, 5, "* BALATRO SHOP *", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    # Money Badge
    cv.draw_rounded_panel(8, 22, 94, 24, col_rgb(24, 18, 4), COLOR_GOLD_COIN)
    cv.draw_string_shadow(16, 30, "$15 CASH", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    # Jokers Info
    cv.draw_string(114, 24, "JOKERS: 3/5", COLOR_TEXT_WHITE)
    cv.draw_string(114, 36, "CONSUMABLES: 1/2", COLOR_TEXT_WHITE)

    # Draw Owned Jokers in a neat row
    for i in range(5):
        jx = 8 + i * 49
        jy = 54
        if i < 3:
            cv.blit_sprite(jx, jy, 26, 36, jokers_gfx[i * 4])
            cv.draw_string(jx, jy + 38, ["Joker", "Greedy", "Cavend"][i], COLOR_TEXT_WHITE)
        else:
            cv.draw_rounded_panel(jx, jy, 26, 36, COLOR_BOX_DARK, COLOR_BOX_BORDER)
            cv.draw_string(jx + 8, jy + 14, "+", col_rgb(8, 14, 10))

    # Draw Owned Consumables
    for c in range(2):
        cx = 156 + c * 46
        cy = 114
        if c == 0:
            cv.blit_sprite(cx, cy, 26, 36, cons_gfx[22]) # Mercury
            cv.draw_string(cx, cy + 38, "Mercury", COLOR_GOLD_COIN)
        else:
            cv.draw_rounded_panel(cx, cy, 26, 36, COLOR_BOX_DARK, COLOR_BOX_BORDER)
            cv.draw_string(cx + 8, cy + 14, "T", col_rgb(8, 14, 10))

    cv.draw_string(14, 168, "Manage or inspect your deck & inventory", col_rgb(16, 22, 18))
    return cv.img

def render_bottom_screen_shop():
    cv = ScreenCanvas()
    # Slot 0 (Joker: Gros Michel)
    s0_x = 8
    s0_y = 10
    cv.draw_rounded_panel(s0_x, s0_y, 70, 78, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.blit_sprite(s0_x + 22, s0_y + 6, 26, 36, jokers_gfx[12]) # Gros Michel
    cv.draw_string(s0_x + 4, s0_y + 46, "Gros Michel", COLOR_TEXT_WHITE)
    cv.draw_rounded_panel(s0_x + 4, s0_y + 58, 62, 16, col_rgb(4, 18, 8), COLOR_GOLD_COIN)
    cv.draw_string(s0_x + 8, s0_y + 62, "BUY: $5", COLOR_GOLD_COIN)

    # Slot 1 (Consumable: The Empress)
    s1_x = 84
    s1_y = 10
    cv.draw_rounded_panel(s1_x, s1_y, 70, 78, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.blit_sprite(s1_x + 22, s1_y + 6, 26, 36, cons_gfx[3]) # Empress
    cv.draw_string(s1_x + 4, s1_y + 46, "The Empress", COLOR_TEXT_WHITE)
    cv.draw_rounded_panel(s1_x + 4, s1_y + 58, 62, 16, col_rgb(4, 18, 8), COLOR_GOLD_COIN)
    cv.draw_string(s1_x + 8, s1_y + 62, "BUY: $3", COLOR_GOLD_COIN)

    # Booster Pack Slot (Arcana)
    sp_x = 160
    sp_y = 10
    cv.draw_rounded_panel(sp_x, sp_y, 88, 78, col_rgb(14, 8, 22), col_rgb(24, 12, 31))
    cv.draw_string_shadow(sp_x + 8, sp_y + 8, "Arcana", COLOR_GOLD_COIN, COLOR_BOX_DARK)
    cv.draw_string(sp_x + 8, sp_y + 22, "Booster Pack", COLOR_TEXT_WHITE)
    cv.draw_rounded_panel(sp_x + 8, sp_y + 52, 72, 18, col_rgb(28, 16, 4), COLOR_GOLD_COIN)
    cv.draw_string(sp_x + 14, sp_y + 56, "OPEN ($4)", COLOR_GOLD_COIN)

    # Voucher Slot
    sv_x = 8
    sv_y = 94
    cv.draw_rounded_panel(sv_x, sv_y, 146, 38, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.draw_string(sv_x + 6, sv_y + 5, "Voucher: Upgrade", COLOR_GOLD_COIN)
    cv.draw_string(sv_x + 6, sv_y + 19, "BUY VOUCHER ($10)", COLOR_TEXT_WHITE)

    # Inventory / Sell Button
    sinv_x = 160
    sinv_y = 94
    cv.draw_rounded_panel(sinv_x, sinv_y, 88, 38, col_rgb(24, 16, 4), COLOR_GOLD_COIN)
    cv.draw_string_shadow(sinv_x + 12, sinv_y + 6, "INVENTORY", COLOR_GOLD_COIN, COLOR_BOX_DARK)
    cv.draw_string_shadow(sinv_x + 8, sinv_y + 20, "[SELL ITEMS]", COLOR_TEXT_WHITE, COLOR_BOX_DARK)

    # Bottom Controls: REROLL and NEXT BLIND
    cv.draw_rounded_panel(8, 140, 110, 44, col_rgb(24, 14, 4), COLOR_GOLD_COIN)
    cv.draw_string_shadow(18, 156, "REROLL ($5)", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    cv.draw_rounded_panel(124, 140, 124, 44, col_rgb(4, 26, 10), COLOR_GOLD_COIN)
    cv.draw_string_2x_shadow(134, 152, "NEXT BLIND", COLOR_TEXT_WHITE, COLOR_BOX_DARK)

    return cv.img

def render_bottom_screen_shop_inv():
    cv = ScreenCanvas()
    # Top Bar
    cv.draw_rounded_panel(6, 4, 96, 20, COLOR_BOX_DARK, COLOR_GOLD_COIN)
    cv.draw_string(10, 9, "< BACK TO SHOP", COLOR_GOLD_COIN)

    cv.draw_rounded_panel(108, 4, 86, 20, COLOR_BOX_DARK, COLOR_BOX_BORDER)
    cv.draw_string_shadow(116, 9, "SELL ITEMS", COLOR_TEXT_WHITE, COLOR_BOX_DARK)

    cv.draw_rounded_panel(200, 4, 50, 20, col_rgb(24, 18, 4), COLOR_GOLD_COIN)
    cv.draw_string_shadow(208, 9, "$15", COLOR_GOLD_COIN, COLOR_BOX_DARK)

    # Owned Jokers (5 slots)
    cv.draw_string(8, 28, "TAP A JOKER TO SELL:", col_rgb(18, 24, 18))
    for j in range(5):
        jx = 4 + j * 50
        jy = 38
        cv.draw_rounded_panel(jx, jy, 48, 52, COLOR_BOX_DARK, COLOR_BOX_BORDER)

        if j < 3:
            cv.blit_sprite(jx + 11, jy + 2, 26, 36, jokers_gfx[j * 4])
            cv.draw_rounded_panel(jx + 3, jy + 36, 42, 13, col_rgb(26, 4, 6), COLOR_MULT_RED)
            cv.draw_string(jx + 5, jy + 39, f"SELL ${[2, 3, 2][j]}", COLOR_TEXT_WHITE)
        else:
            cv.draw_string(jx + 18, jy + 20, "-", col_rgb(10, 16, 12))

    # Owned Consumables (2 slots)
    cv.draw_string(8, 96, "TAP A CONSUMABLE TO SELL:", col_rgb(18, 24, 18))
    for c in range(2):
        cx = 8 + c * 124
        cy = 106
        cv.draw_rounded_panel(cx, cy, 116, 42, COLOR_BOX_DARK, COLOR_BOX_BORDER)

        if c == 0:
            cv.blit_sprite(cx + 4, cy + 3, 26, 36, cons_gfx[22]) # Mercury
            cv.draw_string(cx + 34, cy + 6, "Mercury", COLOR_GOLD_COIN)
            cv.draw_rounded_panel(cx + 34, cy + 20, 68, 16, col_rgb(26, 4, 6), COLOR_MULT_RED)
            cv.draw_string(cx + 40, cy + 24, "SELL ($1)", COLOR_TEXT_WHITE)
        else:
            cv.draw_string(cx + 38, cy + 16, "(EMPTY)", col_rgb(12, 16, 14))

    # Return to Shop Button
    cv.draw_rounded_panel(24, 154, 208, 32, col_rgb(4, 26, 10), COLOR_GOLD_COIN)
    cv.draw_string_shadow(68, 164, "DONE - RETURN TO SHOP", COLOR_TEXT_WHITE, COLOR_BOX_DARK)
    return cv.img

os.makedirs("build", exist_ok=True)
# 1. Active Round
top_act = render_top_screen_active()
bot_act = render_bottom_screen_active()
dual_act = combine_dual_screen(top_act, bot_act)
dual_act.save("build/balatro_dsi_active_round_preview.png")

# 2. Shop View
top_shop = render_top_screen_shop()
bot_shop = render_bottom_screen_shop()
dual_shop = combine_dual_screen(top_shop, bot_shop)
dual_shop.save("build/balatro_dsi_shop_preview.png")

# 3. Shop Inventory Sell View
bot_shop_inv = render_bottom_screen_shop_inv()
dual_shop_inv = combine_dual_screen(top_shop, bot_shop_inv)
dual_shop_inv.save("build/balatro_dsi_shop_inventory_preview.png")

print("All dual screen previews generated successfully!")

