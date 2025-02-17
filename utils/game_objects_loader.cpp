#include "game_objects_loader.hpp"
#include "SFML/System/Vector2.hpp"
#include "game_object.hpp"
#include <iostream>
#include <json.hpp>
#include <textures_loader.hpp>

#include <fstream>
#include <memory>

using nlohmann::json;

std::shared_ptr<GameObject> ParseGameObjects(std::string_view game_objects_path) {
    std::ifstream file(game_objects_path);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open file " + std::string(game_objects_path));
    }

    json data;
    file >> data;

    if (!data.contains("root")) {
        throw std::runtime_error("There is no \"root\" in " + std::string(game_objects_path));
    }

    std::unordered_map<std::string, std::vector<std::pair<std::string, std::optional<std::string>>>> objects_graph;
    for (const auto& item : data.items()) {
        if (item.key() == "root") {
            continue;
        }

        if (!item.value().contains("parent")) {
            throw std::logic_error("There is no \"parent\" field in " + item.key());
        }

        std::string parent = item.value()["parent"].template get<std::string>();
        std::optional<std::string> parent_phase;
        size_t pos = parent.find(':');
        if (pos != std::string::npos) {
            parent_phase = parent.substr(pos, parent.size() - pos);
            parent = parent.substr(0, pos);
        }

        objects_graph[parent].push_back({item.key(), parent_phase});
    }

    std::function<std::shared_ptr<GameObject>(std::string, const sf::Rect<int>&)> dfs =
        [&objects_graph, &data, &dfs](std::string vertex,
                                      const sf::Rect<int>& parent_sprites_rect) {
            if (!data[vertex].contains("scale")) {
                throw std::logic_error("There is no \"scale\" field in " + vertex);
            }

            if (!data[vertex].contains("coords")) {
                throw std::logic_error("There is no \"coords\" field in " + vertex);
            }

            if (!data[vertex].contains("default_phase")) {
                throw std::logic_error("There is no \"default_phase\" field in " + vertex);
            }

            std::string default_phase = data[vertex]["default_phase"].template get<std::string>();
            double scale = data[vertex]["scale"].template get<double>();
            std::array<float, 2> coords =
                data[vertex]["coords"].template get<std::array<float, 2>>();

            std::shared_ptr<GameObject> object = std::make_shared<GameObject>(vertex, scale, default_phase);
            sf::Vector2f pos = {
                parent_sprites_rect.getPosition().x + parent_sprites_rect.width * coords[0],
                parent_sprites_rect.getPosition().y + parent_sprites_rect.height * coords[1]};

            if (!data[vertex].contains("phases")) {
                throw std::logic_error("There is no \"phases\" field in " + vertex);
            }

            for (auto item : data[vertex]["phases"].items()) {
                if (!item.value().contains("type")) {
                    throw std::logic_error("There is no \"type\" field in the " + item.key() + " phase in " + vertex);
                }
                std::string type = item.value()["type"].template get<std::string>();

                if (type == "image") {
                    if (!item.value().contains("source")) {
                        throw std::logic_error("There is no \"source\" field in the " + item.key() + " phase in " + vertex);
                    }

                    sf::Sprite sprite(GetTextute(item.value()["source"].template get<std::string>()));
                    sprite.setPosition(pos);
                    object->AddPhase({std::move(sprite)}, item.key());
                } else if (type == "animation") {
                    if (!item.value().contains("source_dir")) {
                        throw std::logic_error("There is no \"source_dir\" field in the " + item.key() + " phase in " + vertex);
                    }

                    std::string source_dir =
                        item.value()["source_dir"].template get<std::string>();
                    TexturesRef textures = GetTextures(source_dir);
                    std::vector<sf::Sprite> animation;
                    animation.reserve(textures.size());
                    for (auto source : textures) {
                        sf::Sprite sprite(source.get());
                        sprite.setPosition(pos);
                        animation.push_back(std::move(sprite));
                    }
                    object->AddPhase(std::move(animation), item.key());
                } else {
                    throw std::logic_error("Wrong value of \"type\" field in the " + item.key() + " phase in " + vertex);
                }
            }

            for (const auto& child : objects_graph[vertex]) {
                object->AddChild(dfs(child.first, object->GetSpriteRect()), child.second);
            }

            return object;
        };

    return dfs("root", sf::Rect<int>());
}
