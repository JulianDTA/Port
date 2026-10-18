import re

with open('extracted/game.lua', 'r', encoding='utf-8', errors='ignore') as f:
    text = f.read()

lines = text.split('\n')
jokers = []
for line in lines:
    line_s = line.strip()
    if line_s.startswith('j_') and 'pos =' in line_s and 'set =' in line_s:
        key = line_s.split('=')[0].strip()
        name_m = re.search(r'name\s*=\s*["\']([^"\']+)["\']', line_s)
        pos_m = re.search(r'pos\s*=\s*\{\s*x\s*=\s*(\d+)\s*,\s*y\s*=\s*(\d+)\s*\}', line_s)
        cost_m = re.search(r'cost\s*=\s*(\d+)', line_s)
        rarity_m = re.search(r'rarity\s*=\s*(\d+)', line_s)
        if name_m and pos_m:
            name = name_m.group(1)
            x = int(pos_m.group(1))
            y = int(pos_m.group(2))
            cost = int(cost_m.group(1)) if cost_m else 4
            rarity = int(rarity_m.group(1)) if rarity_m else 1
            jokers.append((key, name, cost, rarity, x, y))

print(f"Total Jokers in Balatro: {len(jokers)}")
for j in jokers:
    print(f"{j[0]}: \"{j[1]}\", cost={j[2]}, rarity={j[3]}, pos=({j[4]}, {j[5]})")
