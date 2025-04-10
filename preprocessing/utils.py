def bitboard_to_string(mask: int, inline=False) -> str:
    bitboard_str = format(mask, '081b')
    if inline:
        return bitboard_str
    else:
        lines = [bitboard_str[row * 9:(row + 1) * 9][::-1] for row in range(9)][::-1]
        return "\n" + "\n".join(lines)
