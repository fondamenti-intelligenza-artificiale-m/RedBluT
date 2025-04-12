import os
import subprocess
import sys

os.makedirs(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'processed')), exist_ok=True)

def generate(module_name):
    result = subprocess.run(['python', '-m', f'generate.{module_name}'])
    if result.returncode != 0:
        raise RuntimeError(f'Error in {module_name} (code {result.returncode})')

if __name__ == '__main__':
    try:
        for name in ['loneliness', 'snag', 'move']:
            generate(name)
    except Exception as e:
        sys.exit(1)
