#ifndef SLIDER_HPP
#define SLIDER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class CelPython;

class Slider {
public:
    Slider();
    Slider(CelPython* game, float x, float y, float width, float height, float min_value, float max_value, float value);
    void update();
    void draw(sf::RenderWindow& window);
    float getValue() const;

    CelPython* game;
    float x, y, width, height;
    float min_value, max_value, value;
    sf::RectangleShape rect;
    sf::RectangleShape slider_rect;

    float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }
};

#endif

