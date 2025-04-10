import os

def generate_key(color, position, snag_mask):
    return (color << 23) | (position << 16) | snag_mask

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, '..', 'processed', 'snag.txt')
    with open(input_path, 'r') as f:
        snag_lines = f.readlines()
    output_path = os.path.join(script_dir, '..', 'processed', 'key.txt')
    with open(output_path, 'w') as f:
        color = None
        position = None
        for line in snag_lines:
            if '-' in line:
                f.write(line)
                color = 0 if 'W' in line else 1
                position = int(line.split("-")[1].split(":")[0], 10)
            else:
                snag_mask = int(line.split(":")[0], 10)
                f.write(f"{color}, {position}, {snag_mask}, {generate_key(color, position, snag_mask)}: {0}\n")

if __name__ == "__main__":
    main()