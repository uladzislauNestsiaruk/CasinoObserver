#pragma once

#include "game_object.hpp"
#include <animations_manager.hpp>

std::vector<std::shared_ptr<GameObject>> ParseGameObjects(std::string_view game_objects_path,
                                                          AnimationsManager& animations_manager);
