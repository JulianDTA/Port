#!/usr/bin/env python3
"""
Comprehensive Balatro Asset Generator for Nintendo DSi:
Generates 16-bit RGB555 sprite headers for:
1. 52 Playing Cards + Card Back (balatro_deck_gfx.h)
2. 22 Tarots + 12 Planets + Black Hole (balatro_consumables_gfx.h)
3. 40 Most Common & Powerful Jokers (balatro_jokers_gfx.h)
4. Blind Poker Chips (balatro_chips_gfx.h)
"""
import os
import sys
from PIL import Image

CARD_W = 26
CARD_H = 36
CHIP_SIZE = 34

def rgb888_to_rgb555(r, g, b, a):
    if a < 32:
        return 0x0000
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    return (1 << 15) | r5 | (g5 << 5) | (b5 << 10)

def main():
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    out_dir = os.path.join(base_dir, "include", "gfx")
    os.makedirs(out_dir, exist_ok=True)

    tex_dir = os.path.join(base_dir, "extracted", "resources", "textures", "1x")
    tarots_img = Image.open(os.path.join(tex_dir, "Tarots.png")).convert("RGBA")
    jokers_img = Image.open(os.path.join(tex_dir, "Jokers.png")).convert("RGBA")
    enh_img = Image.open(os.path.join(tex_dir, "Enhancers.png")).convert("RGBA")
    chips_img = Image.open(os.path.join(tex_dir, "BlindChips.png")).convert("RGBA")

    cw, ch = 71, 95
    base_card = enh_img.crop((1 * cw, 0 * ch, 2 * cw, 1 * ch))

    # --- 1. Consumables: 22 Tarots + 12 Planets + Black Hole ---
    consumable_defs = [
        # 22 Tarots (Index 0 to 21)
        ("fool", "The Fool", 0, 0),
        ("magician", "The Magician", 1, 0),
        ("high_priestess", "The High Priestess", 2, 0),
        ("empress", "The Empress", 3, 0),
        ("emperor", "The Emperor", 4, 0),
        ("heirophant", "The Hierophant", 5, 0),
        ("lovers", "The Lovers", 6, 0),
        ("chariot", "The Chariot", 7, 0),
        ("justice", "Justice", 8, 0),
        ("hermit", "The Hermit", 9, 0),
        ("wheel_of_fortune", "The Wheel of Fortune", 0, 1),
        ("strength", "Strength", 1, 1),
        ("hanged_man", "The Hanged Man", 2, 1),
        ("death", "Death", 3, 1),
        ("temperance", "Temperance", 4, 1),
        ("devil", "The Devil", 5, 1),
        ("tower", "The Tower", 6, 1),
        ("star", "The Star", 7, 1),
        ("moon", "The Moon", 8, 1),
        ("sun", "The Sun", 9, 1),
        ("judgement", "Judgement", 0, 2),
        ("world", "The World", 1, 2),

        # 12 Planets (Index 22 to 33)
        ("mercury", "Mercury", 0, 3),
        ("venus", "Venus", 1, 3),
        ("earth", "Earth", 2, 3),
        ("mars", "Mars", 3, 3),
        ("jupiter", "Jupiter", 4, 3),
        ("saturn", "Saturn", 5, 3),
        ("uranus", "Uranus", 6, 3),
        ("neptune", "Neptune", 7, 3),
        ("pluto", "Pluto", 8, 3),
        ("planet_x", "Planet X", 9, 2),
        ("ceres", "Ceres", 8, 2),
        ("eris", "Eris", 3, 2),

        # Spectral
        ("black_hole", "Black Hole", 9, 3)
    ]

    cons_header = os.path.join(out_dir, "balatro_consumables_gfx.h")
    print(f"Generating {cons_header} ({len(consumable_defs)} consumables)...")
    with open(cons_header, "w") as f:
        f.write("// Authentic Balatro Consumables: 22 Tarots + 12 Planets + Black Hole\n")
        f.write("#ifndef BALATRO_CONSUMABLES_GFX_H\n#define BALATRO_CONSUMABLES_GFX_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define BALATRO_CONSUMABLE_COUNT {len(consumable_defs)}\n")
        f.write(f"#define BALATRO_CONS_W {CARD_W}\n")
        f.write(f"#define BALATRO_CONS_H {CARD_H}\n\n")

        f.write(f"static const uint16_t s_balatro_consumables[{len(consumable_defs)}][{CARD_W * CARD_H}] = {{\n")
        for idx, (cid, cname, gx, gy) in enumerate(consumable_defs):
            crop = tarots_img.crop((gx * cw, gy * ch, (gx + 1) * cw, (gy + 1) * ch))
            comp = Image.new("RGBA", (cw, ch), (0, 0, 0, 0))
            comp.paste(base_card, (0, 0))
            comp.alpha_composite(crop)
            ds = comp.resize((CARD_W, CARD_H), Image.Resampling.LANCZOS)
            
            f.write(f"  // [{idx}] {cname} (pos {gx},{gy})\n  {{\n")
            pixels = list(ds.getdata())
            for i, px in enumerate(pixels):
                val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
                f.write(f"0x{val:04X},")
                if (i + 1) % 13 == 0:
                    f.write("\n")
            f.write("  },\n")
        f.write("};\n\n")
        f.write("#endif // BALATRO_CONSUMABLES_GFX_H\n")

    # --- 2. Top 40 Jokers ---
    joker_defs = [
        ("joker", "Joker", 0, 0),
        ("greedy", "Greedy Joker", 6, 1),
        ("lusty", "Lusty Joker", 7, 1),
        ("wrathful", "Wrathful Joker", 8, 1),
        ("gluttonous", "Gluttonous Joker", 9, 1),
        ("jolly", "Jolly Joker", 2, 0),
        ("zany", "Zany Joker", 3, 0),
        ("mad", "Mad Joker", 4, 0),
        ("crazy", "Crazy Joker", 5, 0),
        ("droll", "Droll Joker", 6, 0),
        ("sly", "Sly Joker", 0, 14),
        ("wily", "Wily Joker", 1, 14),
        ("clever", "Clever Joker", 2, 14),
        ("devious", "Devious Joker", 3, 14),
        ("crafty", "Crafty Joker", 4, 14),
        ("half", "Half Joker", 7, 0),
        ("credit_card", "Credit Card", 5, 1),
        ("banner", "Banner", 1, 2),
        ("mystic_summit", "Mystic Summit", 2, 2),
        ("8_ball", "8 Ball", 0, 5),
        ("misprint", "Misprint", 6, 2),
        ("raised_fist", "Raised Fist", 8, 2),
        ("chaos", "Chaos the Clown", 1, 0),
        ("scary_face", "Scary Face", 2, 3),
        ("abstract", "Abstract Joker", 3, 3),
        ("delayed_grat", "Delayed Gratification", 4, 3),
        ("gros_michel", "Gros Michel", 7, 6),
        ("even_steven", "Even Steven", 8, 3),
        ("odd_todd", "Odd Todd", 9, 3),
        ("scholar", "Scholar", 0, 4),
        ("supernova", "Supernova", 2, 4),
        ("ride_the_bus", "Ride the Bus", 1, 6),
        ("blackboard", "Blackboard", 2, 10),
        ("ice_cream", "Ice Cream", 4, 10),
        ("blue_joker", "Blue Joker", 7, 10),
        ("constellation", "Constellation", 9, 10),
        ("green_joker", "Green Joker", 2, 11),
        ("cavendish", "Cavendish", 5, 11),
        ("card_sharp", "Card Sharp", 6, 11),
        ("red_card", "Red Card", 7, 11),
        ("madness", "Madness", 8, 11),
        ("square", "Square Joker", 9, 11),
        ("vampire", "Vampire", 2, 12),
        ("shortcut", "Shortcut", 3, 12),
        ("hologram", "Hologram", 4, 12),
        ("baron", "Baron", 6, 12),
        ("bull", "Bull", 7, 14),
        ("popcorn", "Popcorn", 1, 15),
        ("ramen", "Ramen", 2, 15),
        ("ancient", "Ancient Joker", 7, 15),
        ("blueprint", "Blueprint", 0, 3)
    ]

    jokers_header = os.path.join(out_dir, "balatro_jokers_gfx.h")
    print(f"Generating {jokers_header} ({len(joker_defs)} Jokers)...")
    with open(jokers_header, "w") as f:
        f.write("// Authentic Balatro Jokers (Top 51 Jokers)\n")
        f.write("#ifndef BALATRO_JOKERS_GFX_H\n#define BALATRO_JOKERS_GFX_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define BALATRO_JOKER_COUNT {len(joker_defs)}\n")
        f.write(f"#define BALATRO_JK_W {CARD_W}\n")
        f.write(f"#define BALATRO_JK_H {CARD_H}\n\n")

        f.write(f"static const uint16_t s_balatro_jokers[{len(joker_defs)}][{CARD_W * CARD_H}] = {{\n")
        for idx, (jid, jname, gx, gy) in enumerate(joker_defs):
            crop = jokers_img.crop((gx * cw, gy * ch, (gx + 1) * cw, (gy + 1) * ch))
            comp = Image.new("RGBA", (cw, ch), (0, 0, 0, 0))
            comp.paste(base_card, (0, 0))
            comp.alpha_composite(crop)
            ds = comp.resize((CARD_W, CARD_H), Image.Resampling.LANCZOS)
            
            f.write(f"  // [{idx}] {jname} (pos {gx},{gy})\n  {{\n")
            pixels = list(ds.getdata())
            for i, px in enumerate(pixels):
                val = rgb888_to_rgb555(px[0], px[1], px[2], px[3])
                f.write(f"0x{val:04X},")
                if (i + 1) % 13 == 0:
                    f.write("\n")
            f.write("  },\n")
        f.write("};\n\n")
        f.write("#endif // BALATRO_JOKERS_GFX_H\n")

    print("[SUCCESS] All Balatro Consumable and Joker graphics generated!")

if __name__ == "__main__":
    main()
