#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>

using namespace std;


class CelPython;

class MenuSubItem {
public:
    MenuSubItem();
    MenuSubItem(string id, CelPython* game);
    void update(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu);
    void draw(sf::RenderWindow& window);

    sf::Texture image;
    sf::Sprite sprite;
    std::string id;
    float alpha;
    CelPython* game;
};

class MenuSubCategory {
public:
    MenuSubCategory();
    MenuSubCategory(string id, CelPython* game);
    void update(const sf::Vector2i& mouse, bool mousePressed, int current_menu);
    void handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu);
    void draw(sf::RenderWindow& window);

    sf::Texture image;
    sf::Sprite sprite;
    int id;
    float alpha;
    CelPython* game;
    bool block;
};

class Button {
public:
    Button();
    Button(CelPython* game, const std::string& img, const sf::Vector2f& size, float rot = 0, const sf::Color& tint = sf::Color::White);
    void update(const sf::Vector2i& mouse, bool mousePressed);
    void draw(sf::RenderWindow& window);
    void handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu);

    sf::Texture image;
    sf::Sprite sprite;
    float alpha;
    float brightness;
    sf::Color tint;
    CelPython* game;
};

class MenuButton {
public:
    MenuButton();
    MenuButton(CelPython* game, const std::string& img, const sf::Vector2f& size, float rot = 0, const sf::Color& tint = sf::Color::White);
    void update(const sf::Vector2i& mouse, bool mousePressed);
    void draw(sf::RenderWindow& window);
    void handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu);

    sf::Texture image;
    sf::Sprite sprite;
    float alpha;
    float brightness;
    sf::Color tint;
    CelPython* game;
};

#endif
