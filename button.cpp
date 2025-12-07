#include <SFML/Graphics.hpp>
#include <memory>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>
#include "game.hpp"
#include "button.hpp"
#include "cellinfo.hpp"

using namespace std;



// Implementations

MenuSubItem::MenuSubItem(string id, CelPython* game) : id(id), game(game), alpha(0.5f), image(game->cell_images[id]), sprite(image) {
    
}

bool MenuSubItem::update(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu) {
    if (current_menu == -1 || current_submenu == -1) return false;

    vector<string> current_subcat = cell_cats_new[current_menu][current_submenu];
    
    
    if (find(current_subcat.begin(), current_subcat.end(), id) != current_subcat.end()) {
        int index = find(current_subcat.begin(), current_subcat.end(), this->id) - current_subcat.begin();
        
        auto& parent_button = this->game->toolbar_subcategories[cell_cats_new[this->game->current_menu][this->game->current_submenu][0]];
        this->x = parent_button.x + 20*(index+1);
        this->y = parent_button.y;
        
        this->sprite.setPosition(sf::Vector2f(this->x, this->y));
        //this->sprite.setScale({2.0f, 2.0f});
        this->brightness = 1.0f;
        // NOW getGlobalBounds() will have the correct position
        if (sprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse))) {
            alpha = 1.0f;
            if (mousePressed) {
                this->brightness = 0.5;
            }
            return true;
            
        } else {
            alpha = 0.5f;
            return false;
        }
    }
    
    return false;
}

void MenuSubItem::draw(sf::RenderWindow& window) {
    if (this->game->current_menu == -1 || this->game->current_submenu == -1) return;
    vector<string> current_cat = cell_cats_new[this->game->current_menu][this->game->current_submenu];

    
    if (find(current_cat.begin(), current_cat.end(), id) != current_cat.end()) {
        int index = find(current_cat.begin(), current_cat.end(), this->id) - current_cat.begin();

        // Set texture every frame (SFML sprites are cheap to update)
        this->sprite.setTexture(this->game->cell_images[this->id]);

        
        
        this->sprite.setPosition(sf::Vector2f(x, y));

        
        
        uint8_t gray_value = static_cast<uint8_t>(brightness * 255.0f);
        sf::Color tint(gray_value, gray_value, gray_value, static_cast<uint8_t>(alpha * 255.0f));
        this->sprite.setColor(tint);
        
        window.draw(sprite);
        
    }
    return;

    //cout << "draw";
}

bool MenuSubItem::handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu) {
    if (current_menu == -1 || current_submenu == -1) {
        return false;
    }
    vector<string> current_cat = cell_cats_new[this->game->current_menu][this->game->current_submenu];

    if (find(current_cat.begin(), current_cat.end(), this->id) == current_cat.end() || current_menu == -1) {
        return false;
    }
    if (this->sprite.getLocalBounds().contains(sf::Vector2f(mouse.x, mouse.y)-this->sprite.getPosition())) {
        if (mousePressed) {
            auto cache = find(current_cat.begin(), current_cat.end(), this->id) - current_cat.begin();
            this->game->brush = id;
        } else {
            this->block = true;
        }
        this->alpha = 1;
        return true;
    }
    return false;

}

MenuSubCategory::MenuSubCategory() 
    : id(""), game(nullptr), alpha(0.5f), brightness(1.0f), 
      image(), sprite(image), block(false) {
}

MenuSubCategory::MenuSubCategory(string id, CelPython* game) : id(id), game(game), alpha(0.5f), brightness(1.0f), image(game->cell_images[id]), sprite(image), block(false) {
    //this->image = this->game->cell_images[id];
    //this->sprite = sf::Sprite(image);
}

bool MenuSubCategory::update(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu) {
    if (this->game->current_menu == -1) return false;

    vector<vector<string>> current_cat = cell_cats_new[this->game->current_menu];
    vector<string> allowed = {};
    for (auto subcat: current_cat) {
        allowed.push_back(subcat[0]);
    }
    
    if (find(allowed.begin(), allowed.end(), id) != allowed.end()) {
        int index = find(allowed.begin(), allowed.end(), this->id) - allowed.begin();

        // Set texture and position HERE in update()
        this->sprite.setTexture(this->game->cell_images[this->id]);
        
        this->x = this->game->toolbar_icon_rects[this->game->current_menu].position.x;
        this->y = this->game->toolbar_icon_rects[this->game->current_menu].position.y - 20 * (index) - 27;
        
        this->sprite.setPosition(sf::Vector2f(this->x, this->y));
        //this->sprite.setScale({2.0f, 2.0f});
        this->brightness = 1.0f;
        // NOW getGlobalBounds() will have the correct position
        if (sprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse))) {
            alpha = 1.0f;
            if (mousePressed) {
                this->brightness = 0.5;
            }
            return true;
            
        } else {
            alpha = 0.5f;
            return false;
        }
    }
    
    return false;
}

void MenuSubCategory::draw(sf::RenderWindow& window) {
    if (this->game->current_menu == -1) return;

    vector<vector<string>> current_cat = cell_cats_new[this->game->current_menu];
    vector<string> allowed = {};
    for (auto subcat: current_cat) {
        allowed.push_back(subcat[0]);
    }
    
    if (find(allowed.begin(), allowed.end(), id) != allowed.end()) {
        int index = find(allowed.begin(), allowed.end(), this->id) - allowed.begin();

        // Set texture every frame (SFML sprites are cheap to update)
        this->sprite.setTexture(this->game->cell_images[this->id]);

        this->x = this->game->toolbar_icon_rects[this->game->current_menu].position.x;
        this->y = this->game->toolbar_icon_rects[this->game->current_menu].position.y - 20 * (index) - 27;
        
        this->sprite.setPosition(sf::Vector2f(x, y));

        
        
        uint8_t gray_value = static_cast<uint8_t>(brightness * 255.0f);
        sf::Color tint(gray_value, gray_value, gray_value, static_cast<uint8_t>(alpha * 255.0f));
        this->sprite.setColor(tint);
        
        window.draw(sprite);
        
    }
    return;

    //cout << "draw";
}

bool MenuSubCategory::handleClick(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu) {
    if (current_menu == -1) return false;
    vector<string> allowed;
    for (auto subcat: cell_cats_new[current_menu]) {
        allowed.push_back(subcat[0]);
    }
    if (find(allowed.begin(), allowed.end(), this->id) == allowed.end() || current_menu == -1) {
        return false;
    }
    if (this->sprite.getLocalBounds().contains(sf::Vector2f(mouse.x, mouse.y)-this->sprite.getPosition())) {
        if (mousePressed) {
            auto cache = find(allowed.begin(), allowed.end(), this->id) - allowed.begin();
            if (this->game->current_submenu == cache) {
                this->game->current_submenu = -1;
            } else {
                this->game->current_submenu = cache;
            }
        } else {
            this->block = true;
        }
        this->alpha = 1;
        return true;
    }
    return false;

}

Button::Button() : image(), sprite(image) {}