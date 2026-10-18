#!/usr/bin/env python3
"""
Generate authentic Balatro 16-bit graphics headers for Nintendo DSi:
- 52 Playing Cards (26x36 pixels, 16-bit RGB555)
- 1 Card Back (26x36 pixels)
- 1 Jimbo Joker (26x36 pixels)
- 3 Blind Poker Chips (Small, Big, Boss - 34x34 pixels)
"""
import os
from PIL import Image

CARD_W = 26
CARD_H = 36
CHIP_W = 34
CHIP_H = 34

def rgb888_to_rgb555(r, g, b, a):
    if a < 32:
        return 0x0000 # Transparent
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    return (1 << 15) | r5 | (g5 << 5) | (b5 << 10)

def main():
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    out_dir = os.path.join(base_dir, "include", "gfx")
    os.makedirs(out_dir, exist_ok=True)

    tex_dir = os.path.join(base_dir, "extracted", "resources", "textures", "1x")
    deck_path = os.path.join(tex_dir, "8BitDeck.png")
    enh_path = os.path.join(tex_dir, "Enhancers.png")
    jokers_path = os.path.join(tex_dir, "Jokers.png")
    chips_path = os.path.join(tex_dir, "BlindChips.png")

    deck_img = Image.open(deck_path).convert("RGBA")
    enh_img = Image.open(enh_path).convert("RGBA")
    jokers_img = Image.open(jokers_path).convert("RGBA")
    chips_img = Image.open(chips_path).convert("RGBA")

    cw, ch = 71, 95
    base_card = enh_img.crop((1 * cw, 0 * ch, 2 * cw, 1 * ch))
    card_back_orig = enh_img.crop((0 * cw, 0 * ch, 1 * cw, 1 * ch))

    # --- 1. Generate balatro_deck_gfx.h ---
    deck_header = os.path.join(out_dir, "balatro_deck_gfx.h")
    print(f"Generating {deck_header}...")
    with open(deck_header, "w") as f:
        f.write("// Authentic Balatro Deck Sprites (26x36, 16-bit direct color RGB555)\n")
        f.write("#ifndef BALATRO_DECK_GFX_H\n#define BALATRO_DECK_GFX_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define BALATRO_CARD_W {CARD_W}\n")
        f.write(f"#define BALATRO_CARD_H {CARD_H}\n\n")

        # 52 cards array: [52][CARD_W * CARD_H]
        # Rows: 0 = Hearts, 1 = Clubs, 2 = Diamonds, 3 = Spades
        # Cols: 0 = 2 ... 12 = Ace
        f.write(f"static const uint16_t s_balatro_cards[52][{CARD_W * CARD_H}] = {{\n")
        for r in range(4):
            for c in range(13):
                box = (c * cw, r * ch, (c + 1) * cw, (r + 1) * ch)
                card_front = deck_img.crop(box)
                comp = Image.new("RGBA", (cw, ch), (0, 0, 0, 0))
                comp.paste(base_card, (0, 0))
                comp.alpha_composite(card_front)
                card_ds = comp.resize((CARD_W, CARD_H), Image.Resampling.LANCZOS)
                
                f.write(f"  // Card index {r * 13 + c} (Row {r}, Col {c})\n  {{\n")
                pixels = list(card_ds.getdata())
                for i, px in enumerate(pixels):
                    val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
                    f.write(f"0x{val:04X},")
                    if (i + 1) % 13 == 0:
                        f.write("\n")
                f.write("  },\n")
        f.write("};\n\n")

        # Card Back
        card_back_ds = card_back_orig.resize((CARD_W, CARD_H), Image.Resampling.LANCZOS)
        f.write(f"static const uint16_t s_balatro_card_back[{CARD_W * CARD_H}] = {{\n")
        pixels = list(card_back_ds.getdata())
        for i, px in enumerate(pixels):
            val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
            f.write(f"0x{val:04X},")
            if (i + 1) % 13 == 0:
                f.write("\n")
        f.write("};\n\n")

        # Jimbo Joker (pos 0,0)
        joker_0 = jokers_img.crop((0, 0, cw, ch))
        joker_comp = Image.new("RGBA", (cw, ch), (0, 0, 0, 0))
        joker_comp.paste(base_card, (0, 0))
        joker_comp.alpha_composite(joker_0)
        joker_ds = joker_comp.resize((CARD_W, CARD_H), Image.Resampling.LANCZOS)
        f.write(f"static const uint16_t s_balatro_joker_jimbo[{CARD_W * CARD_H}] = {{\n")
        pixels = list(joker_ds.getdata())
        for i, px in enumerate(pixels):
            val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
            f.write(f"0x{val:04X},")
            if (i + 1) % 13 == 0:
                f.write("\n")
        f.write("};\n\n")

        f.write("#endif // BALATRO_DECK_GFX_H\n")

    # --- 2. Generate balatro_chips_gfx.h ---
    chips_header = os.path.join(out_dir, "balatro_chips_gfx.h")
    print(f"Generating {chips_header}...")
    with open(chips_header, "w") as f:
        f.write("// Authentic Balatro Blind Poker Chips (34x34, 16-bit RGB555)\n")
        f.write("#ifndef BALATRO_CHIPS_GFX_H\n#define BALATRO_CHIPS_GFX_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define BALATRO_CHIP_SIZE {CHIP_W}\n\n")

        # Small Blind Chip (pos 0, 0)
        sb_crop = chips_img.crop((0, 0, CHIP_W, CHIP_H))
        f.write(f"static const uint16_t s_chip_small_blind[{CHIP_W * CHIP_H}] = {{\n")
        for i, px in enumerate(list(sb_crop.getdata())):
            val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
            f.write(f"0x{val:04X},")
            if (i + 1) % 17 == 0:
                f.write("\n")
        f.write("};\n\n")

        # Big Blind Chip (pos 0, 34)
        bb_crop = chips_img.crop((0, CHIP_H, CHIP_W, CHIP_H * 2))
        f.write(f"static const uint16_t s_chip_big_blind[{CHIP_W * CHIP_H}] = {{\n")
        for i, px in enumerate(list(bb_crop.getdata())):
            val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
            f.write(f"0x{val:04X},")
            if (i + 1) % 17 == 0:
                f.write("\n")
        f.write("};\n\n")

        # Boss Blind Chip (pos 0, 68)
        boss_crop = chips_img.crop((0, CHIP_H * 2, CHIP_W, CHIP_H * 3))
        f.write(f"static const uint16_t s_chip_boss_blind[{CHIP_W * CHIP_H}] = {{\n")
        for i, px in enumerate(list(boss_crop.getdata())):
            val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
            f.write(f"0x{val:04X},")
            if (i + 1) % 17 == 0:
                f.write("\n")
        f.write("};\n\n")

        f.write("#endif // BALATRO_CHIPS_GFX_H\n")

    print("[SUCCESS] All authentic Balatro graphics headers generated!")

if __name__ == "__main__":
    main()
