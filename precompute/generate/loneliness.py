import os

from utils.utils import *

def to_upper_lower(value: int) -> tuple[int, int]:
    lower = value & 0xFFFFFFFFFFFFFFFF
    upper = (value >> 64) & 0xFFFFFFFFFFFFFFFF
    return upper, lower

def format_bitboard_array_cpp(name: str, values: dict[int, int]) -> str:
    lines = [f"constexpr Bitboard {name}[81] = {{"]
    for i in range(81):
        if i in values:
            upper, lower = to_upper_lower(values[i])
        else:
            upper, lower = 0, 0
        lines.append(f"    /* {i:02} */ Bitboard(0x{upper:X}, 0x{lower:X}),")
    lines.append("};\n")
    return "\n".join(lines)

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
    output_path = os.path.join(script_dir, '..', 'computed', 'loneliness.txt')
    with open(output_path, 'w') as f:
        for side, index, bitboard in loneliness_bitboards:
            f.write(f'{side}-{index:02d}: {bitboard_to_string(bitboard)}\n')
    white_map = {}
    black_map = {}
    for side, index, bitboard in loneliness_bitboards:
        if side == 'W':
            white_map[index] = bitboard
        elif side == 'B':
            black_map[index] = bitboard
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_path = os.path.join(script_dir, '..', 'computed', 'loneliness.cpp')
    with open(output_path, 'w') as f:
        f.write(format_bitboard_array_cpp("movesAloneWhite", white_map))
        f.write("\n")
        f.write(format_bitboard_array_cpp("movesAloneBlack", black_map))

if __name__ == '__main__':
    main()
