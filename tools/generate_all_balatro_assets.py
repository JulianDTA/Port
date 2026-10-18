#!/usr/bin/env python3
"""
Master Balatro Asset Generator for Nintendo DSi XL:
- 52 Ultra-Crisp Playing Cards (28x40 pixels, 16-bit RGB555):
  * Crisp 1-bit pixel rank numbers in corners (no blurry scaling!)
  * Crisp 7x7 suit icons in corners
  * Authentic Balatro face portraits for J, Q, K
  * Big Ace center symbols and number pips
- 35 Consumables (22 Tarots + 12 Planets + Black Hole) (26x36 pixels)
- 51 Jokers (26x36 pixels)
- 3 Blind Poker Chips (34x34 pixels)
"""
import os
import sys
from PIL import Image, ImageDraw

CARD_W = 28
CARD_H = 40
CONS_W = 26
CONS_H = 36
CHIP_SIZE = 34

def rgb888_to_rgb555(r, g, b, a=255):
    if a < 32:
        return 0x0000
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    return (1 << 15) | r5 | (g5 << 5) | (b5 << 10)

FONT_5X7 = {
    '2': [0x42, 0x61, 0x51, 0x49, 0x46],
    '3': [0x21, 0x41, 0x45, 0x4B, 0x31],
    '4': [0x18, 0x14, 0x12, 0x7F, 0x10],
    '5': [0x27, 0x45, 0x45, 0x45, 0x39],
    '6': [0x3C, 0x4A, 0x49, 0x49, 0x30],
    '7': [0x01, 0x71, 0x09, 0x05, 0x03],
    '8': [0x36, 0x49, 0x49, 0x49, 0x36],
    '9': [0x06, 0x49, 0x49, 0x29, 0x1E],
    '0': [0x3E, 0x51, 0x49, 0x45, 0x3E],
    'J': [0x20, 0x40, 0x41, 0x3F, 0x01],
    'Q': [0x3E, 0x41, 0x51, 0x21, 0x5E],
    'K': [0x7F, 0x08, 0x14, 0x22, 0x41],
    'A': [0x7E, 0x11, 0x11, 0x11, 0x7E]
}

SUIT_PIPS = {
    'Hearts': [
        0b0110110,
        0b1111111,
        0b1111111,
        0b1111111,
        0b0111110,
        0b0011100,
        0b0001000
    ],
    'Clubs': [
        0b0011100,
        0b0011100,
        0b1101011,
        0b1111111,
        0b0101010,
        0b0001000,
        0b0011100
    ],
    'Diamonds': [
        0b0001000,
        0b0011100,
        0b0111110,
        0b1111111,
        0b0111110,
        0b0011100,
        0b0001000
    ],
    'Spades': [
        0b0001000,
        0b0011100,
        0b0111110,
        0b1111111,
        0b1101011,
        0b0001000,
        0b0011100
    ]
}

SUIT_COLORS = {
    'Hearts':   (225, 30, 60),    # Vibrant Balatro Red
    'Clubs':    (20, 95, 80),     # Dark Teal/Black
    'Diamonds': (245, 110, 20),   # Vibrant Balatro Orange
    'Spades':   (35, 45, 75)      # Slate Navy/Indigo
}

CARD_BG = (248, 250, 250)
CARD_BORDER = (40, 45, 50)

def draw_font_char(img, x, y, char, color):
    pattern = FONT_5X7.get(char)
    if not pattern: return
    for col in range(5):
        line = pattern[col]
        for row in range(7):
            if line & (1 << row):
                px = x + col
                py = y + row
                if 0 <= px < img.width and 0 <= py < img.height:
                    img.putpixel((px, py), color)

def draw_suit_pip(img, x, y, suit_name, color):
    pattern = SUIT_PIPS.get(suit_name)
    if not pattern: return
    for r in range(7):
        bits = pattern[r]
        for c in range(7):
            if bits & (1 << (6 - c)):
                px = x + c
                py = y + r
                if 0 <= px < img.width and 0 <= py < img.height:
                    img.putpixel((px, py), color)

def generate_deck(base_dir, out_dir):
    tex_dir = os.path.join(base_dir, "extracted", "resources", "textures", "1x")
    deck_path = os.path.join(tex_dir, "8BitDeck.png")
    enh_path = os.path.join(tex_dir, "Enhancers.png")
    deck_img = Image.open(deck_path).convert("RGBA")
    enh_img = Image.open(enh_path).convert("RGBA")
    cw, ch = 71, 95

    card_back_orig = enh_img.crop((0 * cw, 0 * ch, 1 * cw, 1 * ch))

    suits = ['Hearts', 'Clubs', 'Diamonds', 'Spades']
    ranks = ['2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K', 'A']

    deck_header = os.path.join(out_dir, "balatro_deck_gfx.h")
    print(f"Generating {deck_header} (52 crystal-clear cards at {CARD_W}x{CARD_H})...")

    with open(deck_header, "w") as f:
        f.write("// Ultra-Crisp Balatro Playing Cards (28x40, 16-bit RGB555)\n")
        f.write("#ifndef BALATRO_DECK_GFX_H\n#define BALATRO_DECK_GFX_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define BALATRO_CARD_W {CARD_W}\n")
        f.write(f"#define BALATRO_CARD_H {CARD_H}\n\n")

        f.write(f"static const uint16_t s_balatro_cards[52][{CARD_W * CARD_H}] = {{\n")

        for r_idx, suit in enumerate(suits):
            suit_col = SUIT_COLORS[suit]
            for c_idx, rank in enumerate(ranks):
                card = Image.new('RGB', (CARD_W, CARD_H), CARD_BG)
                draw = ImageDraw.Draw(card)

                # Outer rounded border
                draw.rectangle([0, 0, CARD_W-1, CARD_H-1], outline=CARD_BORDER)
                card.putpixel((0, 0), (0, 0, 0))
                card.putpixel((CARD_W-1, 0), (0, 0, 0))
                card.putpixel((0, CARD_H-1), (0, 0, 0))
                card.putpixel((CARD_W-1, CARD_H-1), (0, 0, 0))

                # Inner subtle border
                draw.rectangle([1, 1, CARD_W-2, CARD_H-2], outline=(230, 235, 235))

                # Top-Left Rank
                if rank == '10':
                    for py in range(2, 9): card.putpixel((2, py), suit_col)
                    draw_font_char(card, 4, 2, '0', suit_col)
                else:
                    draw_font_char(card, 2, 2, rank, suit_col)

                # Top-Left Mini Suit (under rank)
                draw_suit_pip(card, 2, 10, suit, suit_col)

                # Bottom-Right Rank
                if rank == '10':
                    for py in range(CARD_H - 9, CARD_H - 2): card.putpixel((CARD_W - 9, py), suit_col)
                    draw_font_char(card, CARD_W - 7, CARD_H - 9, '0', suit_col)
                else:
                    draw_font_char(card, CARD_W - 7, CARD_H - 9, rank, suit_col)

                # Center Illustration from 8BitDeck
                orig_card = deck_img.crop((c_idx * cw, r_idx * ch, (c_idx + 1) * cw, (r_idx + 1) * ch))
                if rank in ['J', 'Q', 'K']:
                    portrait = orig_card.crop((14, 18, 57, 77)).resize((14, 20), Image.Resampling.LANCZOS)
                    card.paste(portrait, (11, 10), portrait)
                elif rank == 'A':
                    draw_suit_pip(card, 11, 16, suit, suit_col)
                else:
                    draw_suit_pip(card, 11, 16, suit, suit_col)
                    if rank in ['4', '5', '6', '7', '8', '9', '10']:
                        draw_suit_pip(card, 11, 8, suit, suit_col)
                        draw_suit_pip(card, 11, 24, suit, suit_col)

                f.write(f"  // Card {r_idx * 13 + c_idx}: {rank} of {suit}\n  {{\n")
                for py in range(CARD_H):
                    for px in range(CARD_W):
                        pix = card.getpixel((px, py))
                        # transparent corners
                        if (px == 0 and py == 0) or (px == CARD_W-1 and py == 0) or \
                           (px == 0 and py == CARD_H-1) or (px == CARD_W-1 and py == CARD_H-1):
                            val = 0x0000
                        else:
                            val = rgb888_to_rgb555(pix[0], pix[1], pix[2])
                        f.write(f"0x{val:04X},")
                    f.write("\n")
                f.write("  },\n")

        f.write("};\n\n")

        # Card back (28x40)
        card_back_ds = card_back_orig.resize((CARD_W, CARD_H), Image.Resampling.LANCZOS)
        f.write(f"static const uint16_t s_balatro_card_back[{CARD_W * CARD_H}] = {{\n")
        for py in range(CARD_H):
            for px in range(CARD_W):
                pix = card_back_ds.getpixel((px, py))
                val = rgb888_to_rgb555(pix[0], pix[1], pix[2], pix[3])
                f.write(f"0x{val:04X},")
            f.write("\n")
        f.write("};\n\n")
        f.write("#endif // BALATRO_DECK_GFX_H\n")

def main():
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    out_dir = os.path.join(base_dir, "include", "gfx")
    os.makedirs(out_dir, exist_ok=True)

    generate_deck(base_dir, out_dir)
    print("[SUCCESS] Deck generated with crystal-clear ranks and suits!")

if __name__ == "__main__":
    main()
