#pragma once
#include <SFML/Graphics/Color.hpp>

namespace df {
    enum Color { UNDEFINED_COLOR = -1, BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };
    inline constexpr Color COLOR_DEFAULT = WHITE;

    inline sf::Color toSFColor(Color c) {
        switch (c) {
        case BLACK:   return sf::Color::Black;
        case RED:     return sf::Color::Red;
        case GREEN:   return sf::Color::Green;
        case YELLOW:  return sf::Color::Yellow;
        case BLUE:    return sf::Color::Blue;
        case MAGENTA: return sf::Color::Magenta;
        case CYAN:    return sf::Color::Cyan;
        case WHITE:   return sf::Color::White;
        default:      return sf::Color::White;
        }
    }
}