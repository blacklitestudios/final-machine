#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>



using namespace std;

class Textbox {
public:
    sf::FloatRect rect;
    sf::Color color_active;
    sf::Color color_inactive;
    sf::Color color;
    sf::Font font;
    string text;
    sf::Text txt_surface;
    bool active = false;

    Textbox();
    // defaults: active = (63, 63, 63), inactive = (63, 63, 63) as well
    Textbox(int x, int y, int width, int height, sf::Color color_active, sf::Color color_inactive, sf::Font font, string text, int size);
    // size is automatically generated
    Textbox(int x, int y, int width, int height, sf::Color color_active, sf::Color color_inactive, sf::Font font, string text);

    void handle_event(sf::Event event);
    void update();
    void draw(sf::RenderWindow& window);
};

#endif