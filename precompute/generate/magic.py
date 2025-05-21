import os
import random
from typing import List, Dict

from utils.utils import *

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
    # return (occupancy * magic) >> shift & 0x3FFFF
    idx = (occupancy * magic) >> shift & 0x1FFFF
    if idx >= 1 << 17:  # Verifica se l'indice è troppo grande
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
        
def generate_magic_tables(snags: Dict[str, Dict[int, List[int]]], output_path: str) -> None:
    with open(output_path, 'w') as f:
        for color in ['W', 'B']:
            f.write(f'constexpr uint64_t {color.lower()}MagicNumbers[81] = {{\n')
            magic_numbers = []
            shifts = []
            tables = []

            for i in range(NUM_SQUARES):
                if i not in snags[color]:
                    magic_numbers.append(' ' * 20 + '0x0')
                    shifts.append(' 0')
                    tables.append(None)
                    continue

                max_variations = len(snags[color][i]) - 1
                shift = 81 - max_variations.bit_length()  # Calcola lo shift
                if shift == 65:
                    shift -= 1
                print(f"Shift for square {i}: {shift}")
                magic = find_magic_number(snags[color][i], shift)
                print(f"magic_number: {magic}")
                magic_numbers.append(f'0x{magic:0X}'.rjust(23))  # Formatta magic number con 23 caratteri, aggiungendo spazi
                shifts.append(f'{shift:2d}')

                # Calcola la dimensione della tabella
                table_size = 1 << (81 - shift)  # Il numero di entries nella tabella
                print(f"table_size: {table_size}\n")
                
                # Usa un dizionario per mappare solo gli indici validi
                table = {}

                # Popolare la tabella
                for s in snags[color][i]:
                    idx = index_from_magic(s, magic, shift)
                    move_bitboard = calculate_move_bitboard(0 if color == 'W' else 1, i, s)
                    table[idx] = move_bitboard
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

            # Scrivi move table (le tabelle di lookup)
            f.write(f'constexpr uint64_t {color.lower()}MoveTable[81][131072] = {{\n')
            for i in range(NUM_SQUARES):
                table = tables[i]
                f.write(f'    {{ // Square {i}\n')
                if table is not None:
                    for idx, value in table.items():
                        f.write(f'        0x{idx:05X}: {value},\n')
                else:
                    f.write('        0,  // No valid moves\n')
                f.write('    },\n')

            f.write('};\n')

def main() -> None:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, '..', 'computed', 'snag.txt')
    output_path = os.path.join(script_dir, '..', 'computed', 'magic.txt')

    snags = load_snag_bitboards(input_path)
    generate_magic_tables(snags, output_path)

if __name__ == '__main__':
    main()