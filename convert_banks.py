#!/usr/bin/env python3
"""
Converts GMO alternate firmware banks (.ino + .h wavetables) to binary bank files.
Each bank file: [num_wavetables: 1 byte][wavetable_0..N: each 16384 int8 bytes]
"""

import os
import re
import struct
import math

WAVETABLE_SIZE = 16384
INO_DIR = "/Users/ross/Documents/GMO/AlternateFirmware/AlternateSampleBanks"
WAV_DIR = "/Users/ross/Documents/GMO/AlternateFirmware/Wavetables"
OUT_DIR = "/Users/ross/Documents/VCVrack-submission/res/wavetables/banks"

os.makedirs(OUT_DIR, exist_ok=True)

def resample(data, target_size):
    """Linear interpolation resample to target_size."""
    src_len = len(data)
    if src_len == target_size:
        return data
    out = []
    for i in range(target_size):
        pos = i * (src_len - 1) / (target_size - 1)
        lo = int(pos)
        hi = min(lo + 1, src_len - 1)
        frac = pos - lo
        val = data[lo] * (1.0 - frac) + data[hi] * frac
        out.append(int(round(val)))
    return out

def parse_wavetable_h(filepath):
    """Parse a _int8.h file, return list of int8 values."""
    with open(filepath, 'r') as f:
        content = f.read()
    # Find NUM_CELLS
    m = re.search(r'#define\s+\w+_NUM_CELLS\s+(\d+)', content)
    num_cells = int(m.group(1)) if m else None
    # Extract the DATA array contents
    m = re.search(r'_DATA\s*\[\s*\]\s*=\s*\{([^}]+)\}', content, re.DOTALL)
    if not m:
        raise ValueError(f"No DATA array found in {filepath}")
    raw = m.group(1).strip()
    values = [int(x.strip()) for x in raw.split(',') if x.strip()]
    if num_cells and len(values) != num_cells:
        print(f"  Warning: {os.path.basename(filepath)} NUM_CELLS={num_cells} but got {len(values)} values")
    return values

def find_wavetable_file(filename):
    """Case-insensitive filename lookup in WAV_DIR."""
    target = filename.lower()
    for f in os.listdir(WAV_DIR):
        if f.lower() == target:
            return os.path.join(WAV_DIR, f)
    return None

def parse_speed_range(ino_path):
    """Extract (speedMin, speedMax) from mapfloat pitch line."""
    with open(ino_path, 'r') as f:
        content = f.read()
    m = re.search(r'mapfloat\s*\(\s*\w+\s*,\s*[\d.]+\s*,\s*[\d.]+\s*,\s*([\d.]+)\s*,\s*([\d.]+)\s*\)', content)
    if m:
        return float(m.group(1)), float(m.group(2))
    return 0.05, 3.0  # fallback to VCV default

def parse_ino_includes(ino_path):
    """Extract wavetable filenames from #include lines in .ino file."""
    with open(ino_path, 'r') as f:
        content = f.read()
    # Match: #include <samples/wavetables/foo_int8.h> or <samples/Wavetables/foo_int8.h>
    pattern = re.compile(r'#include\s*<samples/[Ww]avetables/([^>]+)>', re.IGNORECASE)
    return pattern.findall(content)

DRUM_MACHINE_STEMS = {
    "GMO_Acetone_Rhythm_King", "GMO_Boss_DR_55", "GMO_Casio_MT_500",
    "GMO_Casio_RZ_1", "GMO_Casio_Sk_1", "GMO_Commodore_64", "GMO_Denon_CRB_90",
    "GMO_EMU_E_Drum", "GMO_Korg_DDM_220", "GMO_Linn_LM_1", "GMO_Roland_CR_78",
    "GMO_Sequential_Circuits_Drumtraks", "GMO_Vermona_DRM_1",
}
PERCUSSION_STEMS = {
    "GMO_Congas", "GMO_Jazz_kit", "GMO_SNARES", "GMO_TABLA", "GMO_TOM", "GMO_kicks",
}
# Everything else (former melodic, voice, animals, farts, tablabolmouth) → Other

def bank_category(stem):
    if "Artist_Series" in stem or stem == "GMO_Eat_Static":
        return "Artist Series"
    if stem in DRUM_MACHINE_STEMS:
        return "Drum Machines"
    if stem in PERCUSSION_STEMS:
        return "Percussion"
    return "Other"

def bank_name_from_filename(filename):
    """
    GMO_Casio_MT_500 -> Casio MT 500
    GMO_Artist_Series_Baseck_Dogs_And_Lasers -> Baseck: Dogs and Lasers
    """
    stem = os.path.splitext(filename)[0]
    # Strip GMO_ prefix
    if stem.startswith("GMO_"):
        stem = stem[4:]
    # Handle Artist Series
    if stem.startswith("Artist_Series_"):
        rest = stem[len("Artist_Series_"):]
        # Split on underscore, first word(s) are artist name
        # Convention: everything after last meaningful break is the track name
        # We treat the first "word group" before the track title as artist
        # Look for pattern: artist name then track name
        # Simple heuristic: split into words, first 1-2 that form artist name
        # Actually just do: find the split between artist and title
        # The filenames use _ for spaces throughout, so we need a heuristic.
        # Best approach: first word = artist first name (if next word looks like last name, include it)
        # For these specific files, the artist names are known multi-word names.
        # Use a simple approach: split on _ and rejoin with spaces, first word capitalized
        words = rest.split('_')
        # Detect if second word is likely a last name (capitalized, no common English words)
        common_words = {'and', 'or', 'the', 'a', 'an', 'of', 'in', 'on', 'at', 'to', 'for',
                       'dogs', 'lasers', 'static', 'hits', 'kit', 'bank', 'series'}
        # Find where artist name ends and title begins
        # Artist name ends before a word that looks like a title word or after 2 name-like words
        artist_end = 1
        if len(words) > 1 and words[1].lower() not in common_words and words[1][0].isupper():
            artist_end = 2
        artist = ' '.join(words[:artist_end])
        title = ' '.join(words[artist_end:])
        # Lowercase connector words in title
        title_words = []
        for i, w in enumerate(title.split()):
            if i > 0 and w.lower() in {'and', 'or', 'the', 'a', 'an', 'of', 'in', 'on', 'at', 'to', 'for'}:
                title_words.append(w.lower())
            else:
                title_words.append(w)
        title = ' '.join(title_words)
        if title:
            return f"{artist}: {title}"
        else:
            return artist
    else:
        name = stem.replace('_', ' ')
        letters_only = ''.join(c for c in name if c.isalpha())
        if letters_only and letters_only == letters_only.upper():
            name = name.title()
        # Specific renames
        name_map = {
            "Tablabolmouth": "Tabla Bols (mouth)",
            "TABLABOLMOUTH": "Tabla Bols (mouth)",
        }
        name = name_map.get(name, name)
        # Ensure first letter is capitalized
        if name:
            name = name[0].upper() + name[1:]
        return name

# Process all .ino files
ino_files = sorted(f for f in os.listdir(INO_DIR) if f.endswith('.ino'))
bank_manifest = []  # list of (filename_stem, display_name, num_wavetables)

for ino_file in ino_files:
    ino_path = os.path.join(INO_DIR, ino_file)
    display_name = bank_name_from_filename(ino_file)
    stem = os.path.splitext(ino_file)[0]

    includes = parse_ino_includes(ino_path)
    if not includes:
        print(f"SKIP {ino_file} — no wavetable includes found")
        continue

    # Cap at 255 wavetables (stored as 1 byte), but realistically cap at 32 for sanity
    includes = includes[:32]

    wavetables = []
    ok = True
    for inc_filename in includes:
        filepath = find_wavetable_file(inc_filename)
        if not filepath:
            print(f"  ERROR: could not find {inc_filename} for {ino_file}")
            ok = False
            break
        try:
            data = parse_wavetable_h(filepath)
            resampled = resample(data, WAVETABLE_SIZE)
            # Clamp to int8 range
            resampled = [max(-128, min(127, v)) for v in resampled]
            wavetables.append(resampled)
        except Exception as e:
            print(f"  ERROR parsing {inc_filename}: {e}")
            ok = False
            break

    if not ok or not wavetables:
        continue

    out_path = os.path.join(OUT_DIR, stem + ".bin")
    with open(out_path, 'wb') as f:
        f.write(struct.pack('B', len(wavetables)))  # num wavetables
        for wt in wavetables:
            f.write(struct.pack(f'{WAVETABLE_SIZE}b', *wt))

    speed_min, speed_max = parse_speed_range(ino_path)
    print(f"OK  {display_name!r:50s} ({len(wavetables)} wt, speed {speed_min}-{speed_max}) -> {stem}.bin")
    bank_manifest.append((stem, display_name, len(wavetables), bank_category(stem), speed_min, speed_max))

# Write manifest header for C++
manifest_path = os.path.join(OUT_DIR, "banks_manifest.hpp")
with open(manifest_path, 'w') as f:
    f.write("// Auto-generated by convert_banks.py — do not edit\n")
    f.write("#pragma once\n")
    f.write("#include <string>\n\n")
    f.write(f"static const int GMO_NUM_BANKS = {len(bank_manifest)};\n\n")
    f.write("struct GmoBankInfo {\n")
    f.write("    const char* filename;  // stem, no extension\n")
    f.write("    const char* displayName;\n")
    f.write("    const char* category;\n")
    f.write("    int numWavetables;\n")
    f.write("    float speedMin;\n")
    f.write("    float speedMax;\n")
    f.write("    bool centerAtMax;  // if true, speedMax is at knob 50% (range doubles above)\n")
    f.write("};\n\n")
    f.write("static const GmoBankInfo GMO_BANKS[] = {\n")
    for stem, name, num, cat, smin, smax in bank_manifest:
        escaped = name.replace('"', '\\"')
        center = "true"  # all alternate banks: speedMax at knob 50%
        f.write(f'    {{"{stem}", "{escaped}", "{cat}", {num}, {smin}f, {smax}f, {center}}},\n')
    f.write("};\n")

print(f"\nWrote {len(bank_manifest)} banks to {OUT_DIR}")
print(f"Manifest: {manifest_path}")
