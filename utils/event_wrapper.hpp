#pragma once

#include "SFML/Window/Event.hpp"
#include "json.hpp"

namespace EventWrapper {
void Wrap(const sf::Event& event, nlohmann::json& data) noexcept;
}; // namespace EventWrapper
