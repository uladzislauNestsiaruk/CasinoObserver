import json
from pathlib import Path

data = {}

base = 'assets/gamblers/'
for place in ('first_place', 'second_place', 'third_place', 'fourth_place', 'fivth_place', "sixth_place"):
    dir = base + place
    for entry in Path(dir).iterdir():
        name = entry.name[:-4]
        if name.startswith('empty'):
            continue

        coords = list()
        if place == 'first_place':
            coords = [0.006, 0.20]
        elif place == 'second_place':
            coords = [0.22, 0.85]
        elif place == 'third_place':
            coords = [0.42, 0.9]
        elif place == 'fourth_place':
            coords = [0.6, 0.85]
        elif place == 'fivth_place':
            coords = [0.77, 0.75]
        else:
            coords = [0.875, 0.21]

        data[place + '_' + name] = {
            'parent': 'root',
            'default_phase': 'empty',
            'phases': {
                'afk': {
                    'source': f'gamblers_{place}_{name}',
                    'type': 'image'
                },
                'empty': {
                    'source': f'gamblers_{place}_empty_{name}',
                    'type': 'image'
                }
            },
            'scale': [1.0, 1.0],
            'coords': coords
        }

with open('assets/game_objects/utils/gamblers_phases.json', 'w', encoding='utf-8') as f:
    json.dump(data, f, ensure_ascii=False, indent=4)