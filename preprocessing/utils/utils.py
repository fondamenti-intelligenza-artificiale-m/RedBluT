BOARD_SIZE = 9
NUM_SQUARES = BOARD_SIZE * BOARD_SIZE

NORD_CAMP = {3, 4, 5, 13}
WEST_CAMP = {27, 36, 37, 45}
EST_CAMP = {35, 43, 44, 53}
SOUTH_CAMP = {67, 75, 76, 77}
CASTLE = {40}

def get_index(col: int, row: int) -> int:
    return row * BOARD_SIZE + col

def get_coordinates(index: int) -> tuple[int, int]:
    return divmod(index, BOARD_SIZE)

def is_valid(index: int) -> bool:
    return 0 <= index < NUM_SQUARES

def bitboard_to_string(mask: int, inline: bool = True) -> str:
    bitboard_str = format(mask, '081b')
    if inline:
        return bitboard_str
    else:
        lines = [bitboard_str[row * 9:(row + 1) * 9][::-1] for row in range(9)][::-1]
        return "\n" + "\n".join(lines)

def bitboard_to_exponents(bitboard: int) -> set[int]:
    exponents = set()
    position = 0
    while bitboard:
        if bitboard & 1:
            exponents.add(position)
        bitboard >>= 1
        position += 1
    return exponents

def generate_bitboard(starting: int, forbidden: set[int]) -> int:
    bitboard = 0
    row, col = get_coordinates(starting)
    r = row - 1
    while r >= 0:
        i = get_index(col, r)
        if i in forbidden:
            break
        bitboard |= 1 << i
        r -= 1
    r = row + 1
    while r < BOARD_SIZE:
        i = get_index(col, r)
        if i in forbidden:
            break
        bitboard |= 1 << i
        r += 1
    c = col - 1
    while c >= 0:
        i = get_index(c, row)
        if i in forbidden:
            break
        bitboard |= 1 << i
        c -= 1
    c = col + 1
    while c < BOARD_SIZE:
        i = get_index(c, row)
        if i in forbidden:
            break
        bitboard |= 1 << i
        c += 1
    return bitboard

def generate_key(color: int, position: int, snag_mask: int) -> int:
    return (color << 23) | (position << 16) | snag_mask