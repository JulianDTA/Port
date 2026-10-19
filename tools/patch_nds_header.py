import sys
import subprocess

def patch_nds(path):
    print(f"[PATCH] Inspecting NDS header of {path}...")
    with open(path, "r+b") as f:
        # Check current unit code
        f.seek(0x12)
        unit_code = f.read(1)[0]
        print(f"[PATCH] Current Unit code: 0x{unit_code:02X}")
        
        # Enforce Unit code = 0x00 (pure NTR/standard DS homebrew mode)
        f.seek(0x12)
        f.write(b'\x00')
        
        # Clear DSi extended header area (0x180 to 0x1DF) so nds-bootstrap
        # will not mistake this for an incomplete/unsigned DSi title
        f.seek(0x180)
        f.write(b'\x00' * 0x60)
        
    # Run ndstool -f to recalculate and fix Header CRC
    try:
        res = subprocess.run(["ndstool", "-f", path], capture_output=True, text=True)
        if res.returncode == 0:
            print(f"[PATCH] Header CRC recalculated and fixed successfully with ndstool.")
        else:
            print(f"[PATCH] ndstool -f warning: {res.stderr}")
    except Exception as e:
        print(f"[PATCH] Error calling ndstool: {e}")

    # Inspect resulting header
    try:
        res = subprocess.run(["ndstool", "-i", path], capture_output=True, text=True)
        if res.returncode == 0:
            for line in res.stdout.splitlines():
                if "Unit code" in line or "Header CRC" in line or "ARM9 entry" in line or "ARM7 entry" in line:
                    print(f"[PATCH]   {line}")
    except Exception as e:
        pass

if __name__ == "__main__":
    if len(sys.argv) > 1:
        patch_nds(sys.argv[1])
    else:
        print("Usage: python patch_nds_header.py <file.nds>")
