#!/usr/bin/env python3
"""
Generate ultra-crisp, crystal-clear Balatro cards for Nintendo DS (28x40 pixels):
- Crisp 1-bit pixel rank numbers in corner (top-left and bottom-right)
- Crisp 7x7 suit icons in corner
- Authentic Balatro center art (J, Q, K face portraits, big Ace symbols, number pips)
- Zero blurry interpolation on numbers and suits!
"""
import os
from PIL import Image, ImageDraw

CARD_W = 28
CARD_H = 40

# 5x7 font bit patterns for Ranks (characters '2'..'9', 'T', 'J', 'Q', 'K', 'A')
FONT_5X7 = {
    '2': [0x42, 0x61, 0x51, 0x49, 0x46],
    '3': [0x21, 0x41, 0x45, 0x4B, 0x31],
    '4': [0x18, 0x14, 0x12, 0x7F, 0x10],
    '5': [0x27, 0x45, 0x45, 0x45, 0x39],
    '6': [0x3C, 0x4A, 0x49, 0x49, 0x30],
    '7': [0x01, 0x71, 0x09, 0x05, 0x03],
    '8': [0x36, 0x49, 0x49, 0x49, 0x36],
    '9': [0x06, 0x49, 0x49, 0x29, 0x1E],
    'T': [0x7E, 0x11, 0x11, 0x11, 0x7E], # '0' next to 1
    'J': [0x20, 0x40, 0x41, 0x3F, 0x01],
    'Q': [0x3E, 0x41, 0x51, 0x21, 0x5E],
    'K': [0x7F, 0x08, 0x14, 0x22, 0x41],
    'A': [0x7E, 0x11, 0x11, 0x11, 0x7E]
}

# 7x7 Suit bit patterns (7 rows of 7 bits)
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
CARD_SHADOW = (15, 25, 20)

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

def main():
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    tex_dir = os.path.join(base_dir, "extracted", "resources", "textures", "1x")
    deck_path = os.path.join(tex_dir, "8BitDeck.png")
    deck_img = Image.open(deck_path).convert("RGBA")
    cw, ch = 71, 95

    # Suits order: Hearts, Clubs, Diamonds, Spades
    suits = ['Hearts', 'Clubs', 'Diamonds', 'Spades']
    ranks = ['2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A']

    # Grid of all 52 cards: 13 cols x 4 rows
    grid = Image.new('RGB', (13 * CARD_W, 4 * CARD_H), (20, 35, 28))

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
            if rank == 'T':
                # Draw "10"
                # draw '1'
                for py in range(2, 9): card.putpixel((2, py), suit_col)
                # draw '0'
                draw_font_char(card, 4, 2, 'T', suit_col)
            else:
                draw_font_char(card, 2, 2, rank, suit_col)

            # Top-Left Mini Suit (under rank)
            draw_suit_pip(card, 2, 10, suit, suit_col)

            # Bottom-Right Rank
            if rank == 'T':
                for py in range(CARD_H - 9, CARD_H - 2): card.putpixel((CARD_W - 9, py), suit_col)
                draw_font_char(card, CARD_W - 7, CARD_H - 9, 'T', suit_col)
            else:
                draw_font_char(card, CARD_W - 7, CARD_H - 9, rank, suit_col)

            # Center Illustration from 8BitDeck
            orig_card = deck_img.crop((c_idx * cw, r_idx * ch, (c_idx + 1) * cw, (r_idx + 1) * ch))
            if rank in ['J', 'Q', 'K']:
                # Face card portrait crop (center area)
                portrait = orig_card.crop((14, 18, 57, 77)).resize((14, 20), Image.Resampling.LANCZOS)
                card.paste(portrait, (11, 10), portrait)
            elif rank == 'A':
                # Big center Ace pip
                draw_suit_pip(card, 11, 16, suit, suit_col)
            else:
                # Number card center pips
                draw_suit_pip(card, 11, 16, suit, suit_col)
                if rank in ['4', '5', '6', '7', '8', '9', 'T']:
                    draw_suit_pip(card, 11, 8, suit, suit_col)
                    draw_suit_pip(card, 11, 24, suit, suit_col)

            grid.paste(card, (c_idx * CARD_W, r_idx * CARD_H))

    os.makedirs('build', exist_ok=True)
    grid.save('build/crisp_cards_preview_28x40.png')
    print('Generated build/crisp_cards_preview_28x40.png')

if __name__ == '__main__':
    main()
