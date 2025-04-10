import os

from utils import bitboard_to_string

def generate_snag_bitboards(loneliness_mask):
    bitboards = []
    valid_positions = [i for i in range(81) if (loneliness_mask & (1 << i)) != 0]
    num_combinations = 2 ** len(valid_positions)
    for i in range(num_combinations):
        bitboard = loneliness_mask
        for snag, pos in enumerate(valid_positions):
            if (i & (1 << snag)) != 0:
                bitboard |= (1 << pos)
            else:
                bitboard &= ~(1 << pos)
        bitboards.append((i, bitboard))
    return bitboards

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, 'loneliness.txt')
    with open(input_path, 'r') as f:
        loneliness_lines = f.readlines()
    output_path = os.path.join(script_dir, 'snag.txt')
    with open(output_path, 'w') as f:
        for line in loneliness_lines:
            bitboard_str = line.split(":")[1].strip()
            loneliness_mask = int(bitboard_str, 2)
            snag_bitboards = generate_snag_bitboards(loneliness_mask)
            f.write(f"{line}")
            for snag, bitboard in snag_bitboards:
                f.write(f"{snag:05d}: {bitboard_to_string(bitboard, True)}\n")

if __name__ == "__main__":
    main()
