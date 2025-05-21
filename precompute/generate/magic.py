import os
import random
from typing import List, Dict

from utils.utils import *

def split_bitboard_128(bb: int) -> tuple[int, int]:
    """Divide un intero grande in upper e lower 64 bit"""
    lower = bb & 0xFFFFFFFFFFFFFFFF
    upper = (bb >> 64) & 0xFFFFFFFFFFFFFFFF
    return upper, lower

def calculate_move_bitboard(color: int, position: int, snag_mask_81: int) -> int:
    if color == 0:
        return generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)
    else:
        if position in NORD_CAMP:
            return generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | SOUTH_CAMP)
        elif position in WEST_CAMP:
            return generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | EST_CAMP)
        elif position in EST_CAMP:
            return generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | WEST_CAMP)
        elif position in SOUTH_CAMP:
            return generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | NORD_CAMP)
        else:
            return generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)

def load_snag_bitboards(file_path: str) -> Dict[str, Dict[int, List[int]]]:
    snag = {'W': {}, 'B': {}}
    with open(file_path, 'r') as f:
        current_color = ''
        current_index = -1
        for line in f:
            line = line.strip()
            if '-' in line:
                parts = line.split(':')
                color_index = parts[0]
                bitstring = parts[1].strip()
                color, index = color_index.split('-')
                index = int(index)
                current_color = color
                current_index = index
                snag[current_color][current_index] = []
            elif ':' in line:
                _, bitstring = line.split(':')
                bitstring = bitstring.strip()
                bitboard = int(bitstring, 2)
                snag[current_color][current_index].append(bitboard)
    return snag

def index_from_magic(occupancy: int, magic: int, shift: int) -> int:
    idx = (occupancy * magic) >> shift & 0x1FFFF
    if idx >= 1 << 17:
        print(f"Index out of range: {idx} (occupancy: {occupancy}, magic: {magic}, shift: {shift})")
    return idx

def find_magic_number(snag: List[int], shift: int, attempts=1_000_000) -> int | None:
    for _ in range(attempts):
        magic = random.getrandbits(81) & random.getrandbits(81) & random.getrandbits(81)
        used = {}
        collision = False
        for occ in snag:
            idx = index_from_magic(occ, magic, shift)
            if idx in used:
                collision = True
                break
            used[idx] = occ
        if not collision:
            return magic
    return None
"""
def generate_magic_tables(snags: Dict[str, Dict[int, List[int]]], output_path: str) -> None:
    with open(output_path, 'w') as f:
        for color in ['W', 'B']:
            f.write(f'constexpr Bitboard {color.lower()}MagicNumbers[81] = {{\n')
            magic_numbers = []
            shifts = []
            tables = []

            for i in range(NUM_SQUARES):
                if i not in snags[color]:
                    magic_numbers.append('Bitboard(0x0, 0x0)')
                    shifts.append('0')
                    tables.append(None)
                    continue

                max_variations = len(snags[color][i]) - 1
                shift = 81 - max_variations.bit_length()
                if shift == 65:
                    shift -= 1
                print(f"Shift for square {i}: {shift}")
                magic = find_magic_number(snags[color][i], shift)
                print(f"magic_number: {magic}")
                upper, lower = split_bitboard_128(magic)
                magic_numbers.append(f'Bitboard(0x{upper:016X}, 0x{lower:016X})')
                shifts.append(str(shift))

                table_size = 1 << (81 - shift)
                print(f"table_size: {table_size}\n")

                table = {}

                for s in snags[color][i]:
                    idx = index_from_magic(s, magic, shift)
                    move_bitboard = calculate_move_bitboard(0 if color == 'W' else 1, i, s)
                    move_upper, move_lower = split_bitboard_128(move_bitboard)
                    table[idx] = (move_upper, move_lower)
                tables.append(table)

            # Scrivi magic numbers
            for j in range(0, 81, 9):
                line = ', '.join(magic_numbers[j:j+9])
                f.write(f'    {line},\n')
            f.write('};\n\n')

            # Scrivi shifts
            f.write(f'constexpr int {color.lower()}MagicShifts[81] = {{\n')
            for j in range(0, 81, 9):
                line = ', '.join(shifts[j:j+9])
                f.write(f'    {line},\n')
            f.write('};\n\n')

            # Scrivi move table
            f.write(f'constexpr Bitboard {color.lower()}MoveTable[81][131072] = {{\n')
            for i in range(NUM_SQUARES):
                table = tables[i]
                f.write(f'    {{ // Square {i}\n')
                if table is not None:
                    for idx, (upper, lower) in table.items():
                        f.write(f'        [{idx}] = Bitboard(0x{upper:016X}, 0x{lower:016X}),\n')
                else:
                    f.write('        // No valid moves\n')
                f.write('    },\n')
            f.write('};\n')
            """

def generate_magic_tables(snags: Dict[str, Dict[int, List[int]]], output_dir: str) -> None:
    for color in ['W', 'B']:
        is_white = (color == 'W')
        color_name = 'White' if is_white else 'Black'
        file_path = os.path.join(output_dir, f"MagicLookUp{color_name}.cpp")

        with open(file_path, "w") as f:
            f.write(f"#include \"MagicLookUp{color_name}.hpp\"\n\n")
            f.write(f"BitBoard MagicLookUp{color_name}[81][131072];\n\n")
            f.write(f"void InitMagicLookUp{color_name}() {{\n")

            for square in range(81):
                if square not in snags[color]:
                    continue  # Nessuna mossa possibile per questa casella

                # Ricava shift e magic number per la casella
                max_variations = len(snags[color][square]) - 1
                shift = 81 - max_variations.bit_length()
                if shift == 65:
                    shift -= 1
                magic = find_magic_number(snags[color][square], shift)

                # Genera tabella per ogni configurazione
                for s in snags[color][square]:
                    idx = index_from_magic(s, magic, shift)
                    move_bb = calculate_move_bitboard(0 if is_white else 1, square, s)
                    upper, lower = split_bitboard_128(move_bb)
                    f.write(f"    MagicLookUp{color_name}[{square}][{idx}] = BitBoard(0x{upper:016X}, 0x{lower:016X});\n")

            f.write("}\n")


def main() -> None:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, '..', 'computed', 'snag.txt')
    output_path = os.path.join(script_dir, '..', 'computed', 'magic.cpp')

    snags = load_snag_bitboards(input_path)
    #generate_magic_tables(snags, output_path)

    output_path = os.path.join(script_dir, '..', 'computed')
    generate_magic_tables(snags, output_path)

if __name__ == '__main__':
    main()
