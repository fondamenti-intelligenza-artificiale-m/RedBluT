import os

from utils import bitboard_to_string

BOARD_SIZE = 9
NUM_SQUARES = BOARD_SIZE * BOARD_SIZE

NORD_CAMP = {3, 4, 5, 13}
WEST_CAMP = {27, 36, 37, 45}
EST_CAMP = {35, 43, 44, 53}
SOUTH_CAMP = {67, 75, 76, 77}
CASTLE = {40}

def get_index(col, row):
    return row * BOARD_SIZE + col

def get_coordinates(index):
    return divmod(index, BOARD_SIZE)

def is_valid(index):
    return 0 <= index < NUM_SQUARES

def generate_loneliness_bitboards(index, forbidden):
    mask = 0
    row, col = get_coordinates(index)
    r = row - 1
    while r >= 0:
        i = get_index(col, r)
        if i in forbidden:
            break
        mask |= 1 << i
        r -= 1
    r = row + 1
    while r < BOARD_SIZE:
        i = get_index(col, r)
        if i in forbidden:
            break
        mask |= 1 << i
        r += 1
    c = col - 1
    while c >= 0:
        i = get_index(c, row)
        if i in forbidden:
            break
        mask |= 1 << i
        c -= 1
    c = col + 1
    while c < BOARD_SIZE:
        i = get_index(c, row)
        if i in forbidden:
            break
        mask |= 1 << i
        c += 1
    return mask

def generate_all_loneliness_bitboards():
    bitboards = []
    for index in range(NUM_SQUARES):
        if index not in NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP:
            bitboard_white = generate_loneliness_bitboards(index, CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)
            bitboards.append(("white", index, bitboard_white))
    for index in range(NUM_SQUARES):
        if index not in CASTLE:
            if index in NORD_CAMP:
                bitboard_black = generate_loneliness_bitboards(index, CASTLE | SOUTH_CAMP)
            elif index in WEST_CAMP:
                bitboard_black = generate_loneliness_bitboards(index, CASTLE | EST_CAMP)
            elif index in EST_CAMP:
                bitboard_black = generate_loneliness_bitboards(index, CASTLE | WEST_CAMP)
            elif index in SOUTH_CAMP:
                bitboard_black = generate_loneliness_bitboards(index, CASTLE | NORD_CAMP)
            else:
                bitboard_black = generate_loneliness_bitboards(index, CASTLE | NORD_CAMP | WEST_CAMP | EST_CAMP | SOUTH_CAMP)
            bitboards.append(("black", index, bitboard_black))
    return bitboards

def main():
    loneliness_bitboards = generate_all_loneliness_bitboards()
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_path = os.path.join(script_dir, '..', 'processed', 'loneliness.txt')
    with open(output_path, 'w') as f:
        for side, index, bitboard in loneliness_bitboards:
            f.write(f"{side[0].upper()}-{index:02d}: {bitboard_to_string(bitboard, True)}\n")

if __name__ == "__main__":
    main()
