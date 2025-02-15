#pragma once

#include <game_object.hpp>

std::unique_ptr<GameObject> ParseGameObjects(std::string_view game_objects_path);