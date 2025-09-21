#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "game.hpp"
#include "button.hpp"
#include "cellinfo.hpp"

using namespace std;



// Implementations

MenuSubItem::MenuSubItem(string id, CelPython* game) : id(id), game(game), alpha(0.5f), image(game->cell_images[id]), sprite(image) {

}

void MenuSubItem::update(const sf::Vector2i& mouse, bool mousePressed, int current_menu, int current_submenu) {
    if (current_menu == -1 || current_submenu == -1) return;

    vector<string> current_subcat = cell_cats_new[current_menu][current_submenu];
    
    if (find(current_subcat.begin(), current_subcat.end(), id) != current_subcat.end()) {
        if (sprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse))) {
            if (mousePressed) {
                game->brush = id; // Assuming 'brush' is a member of CelPython
            }
            alpha = 1.0f;
        } else {
            alpha = 0.5f;
        }
    }
}

void MenuSubItem::draw(sf::RenderWindow& window) {
    static sf::Shader alphaShader;
    static bool loaded = false;
    if (!loaded) {
        const std::string frag = R"(
            uniform sampler2D texture;
            uniform float u_alpha;
            void main() {
                vec4 color = texture2D(texture, gl_TexCoord[0].xy);
                gl_FragColor = vec4(color.rgb, color.a * u_alpha);
            }
        )";
        bool shader_loaded = alphaShader.loadFromMemory(frag, "fragment");
        if (!shader_loaded) {
            throw "";
        }
        loaded = true;
    }
    alphaShader.setUniform("u_alpha", alpha);
    window.draw(sprite, &alphaShader);
}

Button::Button() : image(), sprite(image) {}