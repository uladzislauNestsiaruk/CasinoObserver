#pragma once

#include <game_object.hpp>

std::shared_ptr<GameObject> ParseGameObjects(std::string_view game_objects_path);