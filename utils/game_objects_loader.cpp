#include "game_objects_loader.hpp"
#include "SFML/System/Vector2.hpp"
#include "game_object.hpp"
#include <json.hpp>
#include <textures_loader.hpp>

#include <fstream>
#include <memory>

using nlohmann::json;

std::unique_ptr<GameObject> ParseGameObjects(std::string_view game_objects_path) {
    std::ifstream file(game_objects_path);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open file " + std::string(game_objects_path));
    }

    json data;
    file >> data;

    if (!data.contains("root")) {
        throw std::runtime_error("There is no \"root\" in " + std::string(game_objects_path));
    }

    std::unordered_map<std::string, std::vector<std::string>> objects_graph;
    for (const auto& item : data.items()) {
        if (item.key() == "root") {
            continue;
        }

        if (!item.value().contains("parent")) {
            throw std::logic_error("There is no \"parent\" field in " + item.key());
        }

        objects_graph[item.value()["parent"].template get<std::string>()].push_back(item.key());
    }

    std::function<std::unique_ptr<GameObject>(std::string, const sf::Rect<int>&)> dfs =
        [&objects_graph, &data, &dfs](std::string vertex,
                                      const sf::Rect<int>& parent_sprites_rect) {
            std::unique_ptr<GameObject> object = std::make_unique<GameObject>(vertex);
            if (!data[vertex].contains("type")) {
                throw std::logic_error("There is no \"type\" field in " + vertex);
            }

            if (!data[vertex].contains("coords")) {
                throw std::logic_error("There is no \"coords\" field in " + vertex);
            }

            std::array<float, 2> coords =
                data[vertex]["coords"].template get<std::array<float, 2>>();
            sf::Vector2f pos = {
                parent_sprites_rect.getPosition().x + parent_sprites_rect.width * coords[0],
                parent_sprites_rect.getPosition().y + parent_sprites_rect.height * coords[1]};

            std::string type = data[vertex]["type"].template get<std::string>();
            if (type == "image") {
                if (!data[vertex].contains("source")) {
                    throw std::logic_error("There is no \"source\" field in " + vertex);
                }

                sf::Sprite sprite(GetTextute(data[vertex]["source"].template get<std::string>()));
                sprite.setPosition(pos);
                object->AddSprite(sprite);
            } else if (type == "animation") {
                if (!data[vertex].contains("sources")) {
                    throw std::logic_error("There is no \"sources\" field in " + vertex);
                }

                std::vector<std::string> sources =
                    data[vertex]["sources"].template get<std::vector<std::string>>();
                for (const auto& source : sources) {
                    sf::Sprite sprite(GetTextute(source));
                    sprite.setPosition(pos);
                    object->AddSprite(sprite);
                }
            } else {
                throw std::logic_error("Wrong value of \"type\" field in " + vertex);
            }

            for (std::string child : objects_graph[vertex]) {
                object->AddChild(std::move(dfs(child, object->GetSpriteRect())));
            }

            return object;
        };

    return dfs("root", sf::Rect<int>());
}
