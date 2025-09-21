
#include <bits/stdc++.h>
#include "slider.hpp"
#include "game.hpp"

using namespace std;

Slider::Slider() {}

Slider::Slider(CelPython* game, float x, float y, float width, float height, float min_value, float max_value, float value)
    : game(game), x(x), y(y), width(width), height(height), min_value(min_value), max_value(max_value), value(value) {
    rect.setPosition(sf::Vector2(x, y));
    rect.setSize(sf::Vector2f(width, height));
    rect.setFillColor(sf::Color(63, 63, 63));

    slider_rect.setSize(sf::Vector2f(4, height));
    slider_rect.setPosition(sf::Vector2(lerp(x, x + width, (value - min_value) / (max_value - min_value)), y));
    slider_rect.setFillColor(sf::Color(127, 127, 127));
}

void Slider::update() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(game->window);
        if (rect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse_pos))) {
            value = lerp(min_value, max_value, (mouse_pos.x - x) / width);
            slider_rect.setPosition(sf::Vector2(float(mouse_pos.x - 5), y)); // Center the slider
        }
    }
}

void Slider::draw(sf::RenderWindow& window) {
    window.draw(rect);
    window.draw(slider_rect);
}

float Slider::getValue() const {
    return value;
}

