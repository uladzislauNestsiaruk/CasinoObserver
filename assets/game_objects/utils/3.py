import json

with open("assets/game_objects/utils/gamblers_phases.json", "r", encoding="utf-8") as f:
    data = json.load(f)

keys = list(data.keys())

cpp_code = f'std::array<std::string, {len(keys)}> keys = {{"' + '", "'.join(keys) + '"}};'

print(cpp_code)
