#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <memory>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace std;


class CelPython;

class MenuSubItem {
public:
    MenuSubItem();
    MenuSubItem(string id, CelPython* game);
    bool update(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu);
    bool handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu);
    void draw(sf::RenderWindow& window);

    sf::Texture image;
    sf::Sprite sprite;
    string id;
    float alpha;
    float brightness;
    CelPython* game;
    bool block;
    int x, y;
};

class MenuSubCategory {
public:
    MenuSubCategory();
    MenuSubCategory(string id, CelPython* game);
    bool update(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu);
    bool handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu);
    void draw(sf::RenderWindow& window);

    sf::Texture image;
    sf::Sprite sprite;
    string id;
    float alpha;
    float brightness;
    CelPython* game;
    bool block;
    int x, y;
};

class Button {
public:
    Button();
    Button(CelPython* game, const std::string& img, const sf::Vector2f& size, float rot = 0, const sf::Color& tint = sf::Color::White);
    bool update(const sf::Vector2i& mouse, bool mousePressed);
    void draw(sf::RenderWindow& window);
    bool handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu);

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
    bool update(const sf::Vector2i& mouse, bool mousePressed);
    void draw(sf::RenderWindow& window);
    bool handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu);

    sf::Texture image;
    sf::Sprite sprite;
    float alpha;
    float brightness;
    sf::Color tint;
    CelPython* game;


};

#endif
