from PIL import Image
from pathlib import Path

def gen_empty(base_dir):
    base = f'assets/{base_dir}'
    for subdir in Path(base).iterdir():
        if subdir.is_dir():
            continue

        file = f'{base}/{subdir.name}'
        new_file = f'{base}/empty_{subdir.name}'
        print(f'Processing: {file}')
        image = Image.open(file)
        width, height = image.size
        new_image = Image.new("RGBA", (width, height), (255, 255, 255, 0))
        new_image.save(new_file)

if __name__ == '__main__':
    gen_empty('chips')