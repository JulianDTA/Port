import os
import sys
import zipfile
import shutil
import time

CANDIDATE_PATHS = [
    r"C:\Program Files (x86)\Steam\steamapps\common\Balatro\Balatro.exe",
    r"C:\Program Files\Steam\steamapps\common\Balatro\Balatro.exe",
    r"C:\Program Files (x86)\Steam\steamapps\downloading\2379780\Balatro.exe",
]

def find_balatro_exe():
    for p in CANDIDATE_PATHS:
        if os.path.exists(p) and os.path.getsize(p) > 1024 * 1024:
            return p
    return None

def extract_archive(exe_path, output_dir):
    print(f"[EXTRACT] Target executable: {exe_path}")
    print(f"[EXTRACT] Output directory: {output_dir}")

    os.makedirs(output_dir, exist_ok=True)

    # In Love2D games, the exe contains the Love runtime followed by the zip archive.
    # Python's zipfile can read concatenated archives directly from the file descriptor if valid.
    try:
        with zipfile.ZipFile(exe_path, 'r') as zf:
            namelist = zf.namelist()
            print(f"[EXTRACT] Found {len(namelist)} items in embedded archive.")
            zf.extractall(output_dir)
            print("[EXTRACT] Successfully extracted all contents via standard zip parser.")
            return True
    except Exception as e:
        print(f"[EXTRACT] Standard zip extraction note: {e}")

    # Fallback: scan for zip local file header signature 'PK\x03\x04'
    print("[EXTRACT] Scanning for zip header manually...")
    try:
        with open(exe_path, 'rb') as f:
            data = f.read()

        pos = data.find(b'PK\x03\x04')
        if pos == -1:
            print("[EXTRACT] Error: No zip signature found in executable (file may still be staging/locked by Steam).")
            return False

        print(f"[EXTRACT] Zip header found at offset: {pos}")
        zip_path = os.path.join(output_dir, "balatro_extracted.zip")
        with open(zip_path, 'wb') as f_out:
            f_out.write(data[pos:])

        with zipfile.ZipFile(zip_path, 'r') as zf:
            zf.extractall(output_dir)
            print(f"[EXTRACT] Successfully extracted {len(zf.namelist())} files from offset {pos}.")
            return True
    except PermissionError:
        print("[EXTRACT] File is currently locked by Steam. Waiting for download to finish...")
        return False
    except Exception as e:
        print(f"[EXTRACT] Error during manual extraction: {e}")
        return False

if __name__ == "__main__":
    exe = find_balatro_exe()
    out = os.path.join(os.path.dirname(__file__), "..", "extracted")
    if not exe:
        print("[EXTRACT] Balatro.exe could not be found.")
        sys.exit(1)
    extract_archive(exe, os.path.abspath(out))
