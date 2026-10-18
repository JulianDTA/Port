with open('extracted/card.lua', 'r', encoding='utf-8', errors='ignore') as f:
    text = f.read()

# Let's see how consumables are used in card.lua
idx = text.find("function Card:use_consumeable")
if idx != -1:
    print(text[idx:idx+3500])
else:
    # search for 'use_consumeable'
    for line in text.split('\n'):
        if 'consumeable' in line and 'function' in line:
            print(line)
