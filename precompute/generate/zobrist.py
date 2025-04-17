import os
import random

random.seed(42)
piece_types = 3
board_size = 81

zobrist_white_to_move = random.getrandbits(64)
zobrist_table = [
    [random.getrandbits(64) for _ in range(board_size)]
    for _ in range(piece_types)
]

cpp_output = f"const uint64_t zobristWhiteToMove = 0x{zobrist_white_to_move:016X};\n"
cpp_output += "const uint64_t zobristTable[3][81] = {\n"
for row in zobrist_table:
    cpp_output += "    { " + ", ".join(f"0x{val:016X}" for val in row) + " },\n"
cpp_output += "};\n\n"

script_dir = os.path.dirname(os.path.abspath(__file__))
output_path = os.path.join(script_dir, '..', 'computed', 'zobrist.txt')
with open(output_path, "w") as file:
    file.write(cpp_output)
