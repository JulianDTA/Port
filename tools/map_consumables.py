import re

with open('extracted/game.lua', 'r', encoding='utf-8', errors='ignore') as f:
    text = f.read()

lines = text.split('\n')
for line in lines:
    line_s = line.strip()
    if line_s.startswith('c_') and 'pos =' in line_s and 'set =' in line_s:
        key = line_s.split('=')[0].strip()
        name_m = re.search(r'name\s*=\s*["\']([^"\']+)["\']', line_s)
        pos_m = re.search(r'pos\s*=\s*\{\s*x\s*=\s*(\d+)\s*,\s*y\s*=\s*(\d+)\s*\}', line_s)
        set_m = re.search(r'set\s*=\s*["\']([^"\']+)["\']', line_s)
        cost_m = re.search(r'cost\s*=\s*(\d+)', line_s)
        if name_m and pos_m and set_m:
            name = name_m.group(1)
            x = int(pos_m.group(1))
            y = int(pos_m.group(2))
            s = set_m.group(1)
            cost = int(cost_m.group(1)) if cost_m else 3
            print(f'{key}: name="{name}", set="{s}", cost={cost}, pos=({x}, {y})')
