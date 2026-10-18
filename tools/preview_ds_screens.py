import os
import re
from PIL import Image, ImageDraw, ImageFont

# Screen dimensions
DS_W = 256
DS_H = 192

def bgr555_to_rgb888(val):
    # val is 15-bit (or 16-bit with alpha)
    # COLOR_RGB(r, g, b) = (r & 0x1F) | ((g & 0x1F) << 5) | ((b & 0x1F) << 10) | 0x8000
    r = (val & 0x1F) << 3
    g = ((val >> 5) & 0x1F) << 3
    b = ((val >> 10) & 0x1F) << 3
    return (r, g, b)

print("DS Preview script generator ready.")
