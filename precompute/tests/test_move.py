import unittest
import os

from utils.utils import *

# === Utility Functions ===

def get_input_path() -> str:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(script_dir, '..', 'computed', 'move.txt')

def get_value(key: int) -> str:
    with open(get_input_path(), 'r') as f:
        for line in f:
            current_key, value = line.split(':', 1)
            if current_key.strip() == key:
                return value.strip()
    return ''

# === Test Classes ===

class TestKeyUniqueness(unittest.TestCase):
    def test_unique_keys(self):
        with open(get_input_path(), 'r') as f:
            lines = f.readlines()
        seen_keys = set()
        duplicate_found = False
        for line in lines:
            key = line.split(':')[0].strip()
            if key in seen_keys:
                duplicate_found = True
            else:
                seen_keys.add(key)
        self.assertFalse(duplicate_found, '[TD1] Duplicate keys found!')

class TestValueCorrectness(unittest.TestCase):
    def test_king(self):
        key = str(generate_key(0, 40, 0)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {22, 31, 38, 39, 41, 42, 49, 58}, '[TK1] Incorrect Value Found for King!')
        key = str(generate_key(0, 40, 0b01011010)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), set(), '[TK2] Incorrect Value Found for King!')
        key = str(generate_key(0, 40, 0b10100101)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {31, 39, 41, 49}, '[TK3] Incorrect Value Found for King!')

    def test_white(self):
        key = str(generate_key(0, 0, 0b0)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {1, 2, 9, 18}, '[TW1] Incorrect Value Found for White!')
        key = str(generate_key(0, 0, 0b1)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {9, 18}, '[TW2] Incorrect Value Found for White!')
        key = str(generate_key(0, 20, 0b0)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {2, 11, 18, 19, 21, 22, 23, 24, 25, 26, 29, 38, 47, 56, 65, 74}, '[TW3] Incorrect Value Found for White!')
        key = str(generate_key(0, 20, 0b0101010101010101)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {11, 19}, '[TW4] Incorrect Value Found for White!')

    def test_black(self):
        key = str(generate_key(1, 80, 0b0)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {62, 71, 78, 79}, '[TB1] Incorrect Value Found for Black!')
        key = str(generate_key(1, 80, 0b1)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {71, 78, 79}, '[TB2] Incorrect Value Found for Black!')
        key = str(generate_key(1, 60, 0b0)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {6, 15, 24, 33, 42, 51, 54, 55, 56, 57, 58, 59, 61, 62, 69, 78}, '[TB3] Incorrect Value Found for Black!')
        key = str(generate_key(1, 60, 0b0101010101010101)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {51, 59}, '[TB4] Incorrect Value Found for Black!')

    def test_camps(self):
        key = str(generate_key(1, 4, 0b0)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {0, 1, 2, 3, 5, 6, 7, 8, 13, 22, 31}, '[TC1] Incorrect Value Found for Black in Camp!')
        key = str(generate_key(1, 75, 0b0)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), {12, 21, 30, 39, 48, 57, 66, 72, 73, 74, 76, 77, 78, 79, 80}, '[TC2] Incorrect Value Found for Black in Camp!')
        key = str(generate_key(1, 52, 0b001100000)).zfill(8)
        self.assertEqual(bitboard_to_exponents(int(get_value(key), 2)), set(), '[TC3] Incorrect Value Found for Black in Camp!')

# === Run Tests ===

if __name__ == '__main__':
    unittest.main()
