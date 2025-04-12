import os

from utils.utils import *

def main() -> None:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, '..', 'processed', 'snag.txt')
    with open(input_path, 'r') as f:
        snag_lines = f.readlines()
    output_path = os.path.join(script_dir, '..', 'processed', 'move.txt')
    with open(output_path, 'w') as f:
        color = None
        position = None
        for line in snag_lines:
            if '-' in line:
                color = 0 if 'W' in line else 1
                position = int(line.split('-')[1].split(':')[0], 10)
            else:
                snag_mask_16 = int(line.split(':')[0], 10)
                snag_mask_81 = int(line.split(':')[1], 2)
                if color == 0:
                    move_bitboard = generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)
                    f.write(f'{generate_key(color, position, snag_mask_16):08d}: {bitboard_to_string(move_bitboard)}\n')
                elif color == 1:
                    if position in NORD_CAMP:
                        move_bitboard = generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | SOUTH_CAMP)
                    elif position in WEST_CAMP:
                        move_bitboard = generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | EST_CAMP)
                    elif position in EST_CAMP:
                        move_bitboard = generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | WEST_CAMP)
                    elif position in SOUTH_CAMP:
                        move_bitboard = generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | NORD_CAMP)
                    else:
                        move_bitboard = generate_bitboard(position, bitboard_to_exponents(snag_mask_81) | CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)
                    f.write(f'{generate_key(color, position, snag_mask_16):08d}: {bitboard_to_string(move_bitboard)}\n')

if __name__ == '__main__':
    main()