#include <fstream>
#include <iostream>
#include <memory>

#include "SFML/System/Vector2.hpp"
#include "game_object.hpp"
#include "game_objects_loader.hpp"
#include <json.hpp>
#include <stdexcept>
#include <textures_loader.hpp>
#include <xlocale/_stdio.h>

using nlohmann::json;
using dep_graph_t =
    std::unordered_map<std::string,
                       std::vector<std::pair<std::string, std::optional<std::string>>>>;

namespace {
template <typename T>
std::optional<T> GetOptionalJsonValue(const json& data, const std::string& key) {
    if (!data.contains(key)) {
        return std::nullopt;
    }

    if (std::is_same_v<T, json> && data[key].is_string()) {
        std::string value = data[key].template get<std::string>();
        if (value[0] == '{') {
            std::ifstream file(GetAssetPrefix() + value.substr(1, value.size() - 2));
            json result;
            file >> result;
            return result;
        }
    }

    return data[key].template get<T>();
}
template <typename T> T GetJsonValue(const json& data, const std::string& key) {
    std::optional<T> result = GetOptionalJsonValue<T>(data, key);
    if (!result.has_value()) {
        throw std::logic_error("There is no \"" + key + "\" field");
    }

    return result.value();
}
} // namespace

void BuildDependencyGraph(const nlohmann::json& data, dep_graph_t& objects_graph,
                          std::vector<std::string>& roots) {
    for (const auto& item : data.items()) {
        if (item.key().substr(0, 2) == "__") {
            continue;
        }

        std::optional<std::string> parent =
            GetOptionalJsonValue<std::string>(item.value(), "parent");
        if (!parent.has_value()) {
            roots.emplace_back(item.key());
            continue;
        }
        std::optional<std::string> parent_phase;
        size_t pos = parent->find(':');
        if (pos != std::string::npos) {
            std::string parent_phases = parent->substr(pos + 1, parent->size() - pos - 1);
            parent = parent->substr(0, pos);
            pos = 0;
            while (pos < parent_phases.size()) {
                size_t next_pos = parent_phases.find(',', pos);
                if (next_pos == std::string::npos) {
                    next_pos = parent_phases.size();
                }
                objects_graph[parent.value()].push_back(
                    {item.key(), parent_phases.substr(pos, next_pos - pos)});
                pos = next_pos + 1;
            }
        } else {
            objects_graph[parent.value()].push_back({item.key(), std::nullopt});
        }
    }
}

std::shared_ptr<GameObject>
DependencyDfs(std::string vertex, const sf::Rect<float>& parent_sprites_rect,
              dep_graph_t& objects_graph, const nlohmann::json& data,
              std::unordered_map<std::string, std::shared_ptr<GameObject>>& tags,
              AnimationsManager& animations_manager) {
    std::string default_phase = GetJsonValue<std::string>(data[vertex], "default_phase");
    std::array<float, 2> coords = GetJsonValue<std::array<float, 2>>(data[vertex], "coords");
    std::optional<std::array<float, 2>> scale =
        GetOptionalJsonValue<std::array<float, 2>>(data[vertex], "scale");
    std::optional<float> rotation = GetOptionalJsonValue<float>(data[vertex], "rotation");

    if (tags.contains(vertex)) {
        return tags[vertex];
    }

    std::shared_ptr<GameObject> object =
        std::make_shared<GameObject>(vertex, default_phase, animations_manager);

    sf::Vector2f pos = {parent_sprites_rect.getPosition().x + parent_sprites_rect.width * coords[0],
                        parent_sprites_rect.getPosition().y +
                            parent_sprites_rect.height * coords[1]};
    object->Move(pos - object->GetPosition());

    tags[vertex] = object;

    json phases = GetJsonValue<json>(data[vertex], "phases");
    for (auto item : phases.items()) {
        std::string type = GetJsonValue<std::string>(item.value(), "type");
        if (type == "image") {
            sf::Sprite sprite(GetTexture(GetJsonValue<std::string>(item.value(), "source")));
            object->AddPhase({std::move(sprite)}, item.key());
        } else if (type == "animation") {
            std::string source_dir = GetJsonValue<std::string>(item.value(), "source_dir");
            TexturesRef textures = GetTextures(source_dir);
            std::vector<sf::Sprite> animation;
            animation.reserve(textures.size());
            for (auto source : textures) {
                sf::Sprite sprite(source.get());
                animation.push_back(std::move(sprite));
            }
            object->AddPhase(std::move(animation), item.key());
        } else {
            throw std::logic_error("Wrong value of \"type\" field in the " + item.key() +
                                   " phase in " + vertex);
        }
    }

    for (const auto& child : objects_graph[vertex]) {
        std::shared_ptr<GameObject> ptr =
            DependencyDfs(child.first, sf::Rect<float>(object->GetPosition(), object->GetSize()),
                          objects_graph, data, tags, animations_manager);
        object->AddChild(ptr, child.second);
    }

    if (scale.has_value()) {
        object->SetScale(sf::Vector2f(scale.value()[0], scale.value()[1]));
    }

    if (rotation.has_value()) {
        object->SetRotation(rotation.value());
        std::cout << object->GetTag() << " " << object->GetRotation() << "\n";
    }

    return object;
}

std::vector<std::shared_ptr<GameObject>> ParseGameObjects(std::string_view game_objects_path,
                                                          AnimationsManager& animations_manager) {
    std::ifstream file(game_objects_path);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open file " + std::string(game_objects_path));
    }

    json data;
    file >> data;

    if (data.contains("__extend__")) {
        std::vector<std::string> extensions =
            GetJsonValue<std::vector<std::string>>(data, "__extend__");
        for (const auto& item : extensions) {
            std::ifstream file(GetAssetPrefix() + item);
            json result;
            file >> result;
            data.merge_patch(result);
        }
    }

    dep_graph_t objects_graph;
    std::vector<std::string> roots;
    BuildDependencyGraph(data, objects_graph, roots);

    std::unordered_map<std::string, std::shared_ptr<GameObject>> tags;

    std::vector<std::shared_ptr<GameObject>> result;
    result.reserve(roots.size());
    for (const std::string& root_rag : roots) {
        result.emplace_back(DependencyDfs(root_rag, sf::Rect<float>(), objects_graph, data, tags,
                                          animations_manager));
    }
    return result;
}
