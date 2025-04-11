import unittest
import os

from utils.utils import *

# === Utility Functions ===

def get_value(key: int) -> str | None:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, '..', 'processed', 'move.txt')
    with open(input_path, 'r') as f:
        for line in f:
            current_key, value = line.split(':', 1)
            if current_key.strip() == key:
                return value.strip()
    return None

# === TestKeyUniqueness Functions ===

def check_unique_keys() -> bool:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, '..', 'processed', 'move.txt')
    with open(input_path, 'r') as f:
        lines = f.readlines()
    seen_keys = set()
    duplicate_found = False
    for line in lines:
        key = line.split(":")[0].strip()
        if key in seen_keys:
            duplicate_found = True
        else:
            seen_keys.add(key)
    return duplicate_found

# === TestValueCorrectness Functions ===

def check_black() -> bool:
    return False

def check_white() -> bool:
    key = str(generate_key(0, 0, 0)).zfill(8)
    assert bitboard_to_exponents(int(get_value(key), 2)) == {1, 2, 9, 18}
    key = str(generate_key(0, 0, 1)).zfill(8)
    print(bitboard_to_exponents(int(get_value(key), 2)))
    assert bitboard_to_exponents(int(get_value(key), 2)) == {9, 18} #???
    return False

def check_king() -> bool:
    key = str(generate_key(0, 40, 0)).zfill(8)
    assert bitboard_to_exponents(int(get_value(key), 2)) == {22, 31, 38, 39, 41, 42, 49, 58}
    return False

# === Test Classes ===

class TestKeyUniqueness(unittest.TestCase):
    def test_unique_keys(self):
        error_found = check_unique_keys()
        self.assertFalse(error_found, "Duplicate keys found!")

class TestValueCorrectness(unittest.TestCase):
    def test_black(self):
        error_found = check_black()
        self.assertFalse(error_found, "Incorrect Value Found for Black!")

    def test_white(self):
        error_found = check_white()
        self.assertFalse(error_found, "Incorrect Value Found for White!")

    def test_king(self):
        error_found = check_king()
        self.assertFalse(error_found, "Incorrect Value Found for King!")

# === Run Tests ===

if __name__ == "__main__":
    unittest.main()
