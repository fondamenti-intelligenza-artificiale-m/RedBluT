import unittest
import os

def check_unique_keys():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, '..', 'processed', 'key.txt')
    with open(input_path, 'r') as f:
        lines = f.readlines()
    seen_keys = set()
    duplicate_found = False
    for line in lines:
        key = line.split(":")[0].split(",")[-1].strip()
        
        if key in seen_keys:
            duplicate_found = True
        else:
            seen_keys.add(key)
    
    return duplicate_found

class TestKeyUniqueness(unittest.TestCase):
    def test_unique_keys(self):
        duplicate_found = check_unique_keys()
        self.assertFalse(duplicate_found, "Duplicate keys found!")

if __name__ == "__main__":
    unittest.main()
