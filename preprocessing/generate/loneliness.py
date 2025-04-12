import os

from utils.utils import *

def generate_all_loneliness_bitboards() -> list[tuple[str, int, int]]:
    bitboards = []
    for index in range(NUM_SQUARES):
        if index not in NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP:
            bitboard_white = generate_bitboard(index, CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)
            bitboards.append(('W', index, bitboard_white))
    for index in range(NUM_SQUARES):
        if index not in CASTLE:
            if index in NORD_CAMP:
                bitboard_black = generate_bitboard(index, CASTLE | SOUTH_CAMP)
            elif index in WEST_CAMP:
                bitboard_black = generate_bitboard(index, CASTLE | EST_CAMP)
            elif index in EST_CAMP:
                bitboard_black = generate_bitboard(index, CASTLE | WEST_CAMP)
            elif index in SOUTH_CAMP:
                bitboard_black = generate_bitboard(index, CASTLE | NORD_CAMP)
            else:
                bitboard_black = generate_bitboard(index, CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)
            bitboards.append(('B', index, bitboard_black))
    return bitboards

def main() -> None:
    loneliness_bitboards = generate_all_loneliness_bitboards()
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_path = os.path.join(script_dir, '..', 'processed', 'loneliness.txt')
    with open(output_path, 'w') as f:
        for side, index, bitboard in loneliness_bitboards:
            f.write(f'{side}-{index:02d}: {bitboard_to_string(bitboard)}\n')

if __name__ == '__main__':
    main()
