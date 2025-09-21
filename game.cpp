#include "game.hpp"
#include "cellinfo.hpp"
extern string ROOT;
extern int DEFAULT_TILE_SIZE;
CelPython::CelPython()
    : window(sf::VideoMode({800, 600}), "CelPython Machine"), 
      window_width(800), 
      window_height(600),
      BACKGROUND(sf::Color(31, 31, 31)),
      tile_size(20.0f),
      TOOLBAR_HEIGHT(54),
      clock(sf::Clock()),
      fps_font(sf::Font()),
      menu_on(false),
      scroll_speed(250.0),
      border_tile(41),
      step_speed(0.2),
      tpu(1),
      grid_width(100),
      grid_height(100),
      current_menu(0),
      suppress_place(false),
      brush("4"),
      brush_dir(0),
      selecting(false),
      select_start({}),
      select_end({}),
      clipboard({}),
      clipboard_above({}),
      clipboard_below({}),
      show_clipboard(false),
      clipboard_start({}),
      clipboard_end({}),
      clipboard_origin({}),
      selected_cell({}),
      cam_x(0.0f),
      cam_y(0.0f),
      mouse_x(0),
      mouse_y(0),
      world_mouse_x(0.0f),
      world_mouse_y(0.0f),
      world_mouse_tile_x(0),
      world_mouse_tile_y(0),
      title(""),
      subtitle(""),
      paused(true),
      tick_number(0),
      bg_cache({}),
      bg_image(ROOT+"textures/bg.png"),
      tools_icon_texture(ROOT+"textures/eraser.png"),
      tools_icon_image(tools_icon_texture),
      basic_icon_texture(ROOT+"textures/push.png"),
      basic_icon_image(basic_icon_texture),
      movers_icon_texture(ROOT+"textures/mover.png"),
      movers_icon_image(movers_icon_texture),
      generators_icon_texture(ROOT+"textures/generator.png"),
      generators_icon_image(generators_icon_texture),
      rotators_icon_texture(ROOT+"textures/rotator_cw.png"),
      rotators_icon_image(rotators_icon_texture),
      forcers_icon_texture(ROOT+"textures/repulsor.png"),
      forcers_icon_image(forcers_icon_texture),
      divergers_icon_texture(ROOT+"textures/diverger.png"),
      divergers_icon_image(divergers_icon_texture),
      destroyers_icon_texture(ROOT+"textures/trash.png"),
      destroyers_icon_image(destroyers_icon_texture),
      misc_icon_texture(ROOT+"textures/ungeneratable.png"),
      misc_icon_image(misc_icon_texture),
      menu_bg(ROOT+"textures/menubg.png"),
      placeable_overlay(ROOT+"textures/effects/placeableoverlay.png"),
      freeze_image(ROOT+"textures/effects/frozen.png"),
      protect_image(ROOT+"textures/effects/protected.png"),
      beep_buffer(ROOT+"audio/beep.wav"),
      beep(beep_buffer),
      destroy_sound_buffer(ROOT+"audio/destroy.ogg"),
      destroy_sound(destroy_sound_buffer),
      move_sound_buffer(ROOT+"audio/move.ogg"),
      move_sound(move_sound_buffer),
      rotate_sound_buffer(ROOT+"audio/rotate.ogg"),
      rotate_sound(rotate_sound_buffer),
      gear_sound_buffer(ROOT+"audio/gear.ogg"),
      gear_sound(gear_sound_buffer),
      music(ROOT+"audio/scattered cells.ogg"),
      result(""),
      puzzlemode(false),
      builtin_puzzle(false),
      buttonz(),
      puzzles_group(),
      topleft_button_group(),
      initial_tags(),
      tags()
    

    
    


    
{
    // prepare window
    window.setTitle("CelPython Machine");
    if (!icon.loadFromFile(ROOT+"textures/icon.png")) {
        throw "failed loading image etc etc";
    }
    window.setIcon(icon);

    // load textures
    for (auto it = cell_names.begin(); it != cell_names.end(); it++) {
        sf::Texture tex;
        if (!tex.loadFromFile(ROOT + "textures/" + (*it).second + ".png")) throw "";
        cell_images_raw.push_back({(*it).first, tex});
    }
    cell_images = map<string, sf::Texture>(cell_images_raw.begin(), cell_images_raw.end());
    if (!this->nonexistant.loadFromFile(ROOT+"textures/nonexistant.png")) throw "";

    // create border tiles
    {
        int i;
        for (i=0; i<this->grid_width; i++) {
            //this->cell_map[{i, -1}]; // FIX LATER
        }
    } // block moment

}



void CelPython::update() {
    static float scroll_accumulator;

    double dt = this->clock.restart().asSeconds();
    if (dt > 0.05) dt = 0.05;
    this->window_height = window.getSize().y;
    this->window_width = window.getSize().x;


    sf::Vector2i mouse_pos = sf::Mouse::getPosition(this->window);
    mouse_x = mouse_pos.x;
    mouse_y = mouse_pos.y;



    // Process events
    while (auto eventOpt = window.pollEvent()) {

        if (eventOpt->is<sf::Event::MouseWheelScrolled>()) {
            const auto& scrollEvent = eventOpt->getIf<sf::Event::MouseWheelScrolled>();
            scroll_accumulator += scrollEvent->delta;

            while (scroll_accumulator >= 1.0f) {
                this->scroll_up(mouse_pos.x, mouse_pos.y);
                scroll_accumulator -= 1.0f;
            }
            while (scroll_accumulator <= -1.0f) {
                this->scroll_down(mouse_pos.x, mouse_pos.y);
                scroll_accumulator += 1.0f;
            }
        }
        if (eventOpt->is<sf::Event::KeyPressed>()) {
            keys[static_cast<int>(eventOpt->getIf<sf::Event::KeyPressed>()->code)] = true;
        }
        if (eventOpt->is<sf::Event::KeyReleased>()) {
            keys[static_cast<int>(eventOpt->getIf<sf::Event::KeyReleased>()->code)] = false;
        }
        if (eventOpt->is<sf::Event::Closed>())
            window.close();
    }

    if (keys[static_cast<int>(sf::Keyboard::Key::W)] || keys[static_cast<int>(sf::Keyboard::Key::Up)]) {
        this->cam_y -= this->scroll_speed*dt;
    }
    if (keys[static_cast<int>(sf::Keyboard::Key::A)] || keys[static_cast<int>(sf::Keyboard::Key::Left)]) {
        this->cam_x -= this->scroll_speed*dt;
    }
    if (keys[static_cast<int>(sf::Keyboard::Key::S)] || keys[static_cast<int>(sf::Keyboard::Key::Down)]) {
        this->cam_y += this->scroll_speed*dt;
    }
    if (keys[static_cast<int>(sf::Keyboard::Key::D)] || keys[static_cast<int>(sf::Keyboard::Key::Right)]) {
        this->cam_x += this->scroll_speed*dt;
    }

    this->world_mouse_x = mouse_x + this->cam_x;
    this->world_mouse_y = mouse_y + this->cam_y;
    this->world_mouse_tile_x = floor(this->world_mouse_x/this->tile_size);
    this->world_mouse_tile_y = floor(this->world_mouse_y/this->tile_size);
    
    // Clear screen
    window.clear({20, 20, 20});


    for (int i = 0; i < this->grid_width; i++) {
        for (int j = 0; j < this->grid_height; j++) {
            if (!(this->tile_size*i-this->cam_x+this->tile_size < 0 
                || this->tile_size*i-this->cam_x > this->window_width 
                || this->tile_size*j-this->cam_y+this->tile_size < 0 
                || this->tile_size*j-this->cam_y > this->window_height)) {
                this->window.draw(this->get_bg(this->tile_size, this->tile_size*i-this->cam_x, this->tile_size*j-this->cam_y));
            }
        }
    }

    if (!this->selecting && !this->show_clipboard && !this->puzzlemode) {
        sf::Sprite brush_image(cell_images[brush]);
        brush_image.setColor({255, 255, 255, 64});
        brush_image.setPosition({this->world_mouse_tile_x*this->tile_size-this->cam_x, this->world_mouse_tile_y*this->tile_size-this->cam_y});
        brush_image.scale({tile_size/DEFAULT_TILE_SIZE, tile_size/DEFAULT_TILE_SIZE});
        window.draw(brush_image);

    }
    window.display();



}

sf::Sprite CelPython::get_bg(int size, float x, float y) {
    sf::Sprite spr(this->bg_image);
    // protect against missing texture
    sf::Vector2u texSize = this->bg_image.getSize();
    if (texSize.x == 0 || texSize.y == 0) {
        spr.setPosition({x, y});
        return spr;
    }

    // Option A — scale to exactly `size` x `size` (fills tile, may stretch)
    float scaleX = static_cast<float>(size) / static_cast<float>(texSize.x);
    float scaleY = static_cast<float>(size) / static_cast<float>(texSize.y);
    spr.setScale({scaleX, scaleY});


    spr.setPosition({x, y});
    return spr;
}

void CelPython::scroll_up(int x, int y) {
    tile_size *= 2.0f;
    if (tile_size > 160.0f) {
        tile_size = 160.0f;
    } else {
        cam_x = (cam_x + x / 2.0f) * 2.0f;
        cam_y = (cam_y + y / 2.0f) * 2.0f;
    }
}

void CelPython::scroll_down(int x, int y) {
    tile_size /= 2.0f;
    if (tile_size < 1.25f) {
        tile_size = 1.25f;
    } else {
        cam_x = cam_x / 2.0f - x / 2.0f;
        cam_y = cam_y / 2.0f - y / 2.0f;
    }
}

void CelPython::play() {
    while (this->window.isOpen()) {
        this->update();
    }
}