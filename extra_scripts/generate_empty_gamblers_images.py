from PIL import Image
from pathlib import Path


base = 'assets/gamblers/'
for place in ('first_place', 'second_place', 'third_place', 'fourth_place', 'fivth_place', "sixth_place"):
    dir = base + place
    for entry in Path(dir).iterdir():
        image = Image.open(f"assets/gamblers/{place}/{entry.name}")
        width, height = image.size
        new_image = Image.new("RGBA", (width, height), (255, 255, 255, 0))
        new_image.save(f"assets/gamblers/{place}/empty_{entry.name}")
