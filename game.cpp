#include "game.hpp"
#include "cellinfo.hpp"
extern string ROOT;
extern int DEFAULT_TILE_SIZE;

pair<int, int> get_deltas(int dir) {
    int dx = 0;
    int dy = 0;
    switch (dir) {
        case 0: dx = 1; break;
        case 1: dy = 1; break;
        case 2: dx = -1; break;
        case 3: dy = -1; break;
    }
    return {dx, dy};
}

bool is_disjoint(const set<string> a, const set<string> b) {
    // Use algorithm without temporary allocations
    for (const auto& element : a) {
        if (b.find(element) != b.end()) {
            return false;
        }
    }
    return true;
}

// Add this to game.cpp or as a utility function

void rotate_sprite_around_point(sf::Sprite& sprite, float angle_degrees, float pivot_x, float pivot_y, sf::Vector2f position) {
    // Set origin to the pivot point
    sprite.setOrigin({pivot_x, pivot_y});
    
    // Set position (this is where the pivot point will be on screen)
    sprite.setPosition(position);
    
    // Apply rotation
    sprite.setRotation(sf::degrees(angle_degrees));
}

CelPython::CelPython()
    : window(sf::VideoMode({800, 600}), "CelPython Machine", sf::Style::Default, sf::State::Windowed, sf::ContextSettings(0, 0, 0, 1, 1, 0, false)), 
      window_width(800), 
      window_height(600),
      BACKGROUND(sf::Color(31, 31, 31)),
      tile_size(16.0f),
      TOOLBAR_HEIGHT(54),
      clock(sf::Clock()),
      fps_font(sf::Font(ROOT+"nokiafc22.ttf")),
      menu_on(false),
      scroll_speed(250.0),
      border_tile("12"),
      step_speed(0.2),
      tpu(1),
      grid_width(100),
      grid_height(100),
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
    // Initialize vector-based cell storage with border
    // Add 2 to each dimension for 1-cell border on all sides
    this->update_timer = this->step_speed;
    int total_cells = (grid_width + 2) * (grid_height + 2);
    cell_map.resize(total_cells);
    above.resize(total_cells);
    below.resize(total_cells);
    initial_cell_map.resize(total_cells);
    fill(cell_map.begin(), cell_map.end(), nullptr);
    
    // prepare window
    window.setTitle("CelPython Machine");
    if (!icon.loadFromFile(ROOT+"textures/icon.png")) {
        throw "failed loading image etc etc";
    }
    window.setIcon(icon);
    // Disable VSync for maximum FPS
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(0); // Uncapped

    // MEMORY FIX: Load textures directly into cell_images map - no duplicate storage
    for (const auto& name : cell_names) {
        sf::Texture tex;
        if (!tex.loadFromFile(ROOT + "textures/" + name.second + ".png")) {
            throw "Failed to load texture: " + name.second;
        }
        this->cell_images[name.first] = std::move(tex); // Move instead of copy
    }
    if (!this->nonexistant.loadFromFile(ROOT+"textures/nonexistant.png")) throw "";

    set<string> allowed;
    for (auto submenu: cell_cats_new) {
        for (auto subcat: submenu) {
            if (subcat.size()) allowed.insert(subcat[0]);
        }
    }

    for (const auto& kv : cell_images) {
        auto cell_id = kv.first;
        this->toolbar_subicons.push_back(MenuSubItem(cell_id, this));
        if (allowed.find(kv.first) == allowed.end()) continue;
        this->toolbar_subcategories[cell_id] = MenuSubCategory(cell_id, this);
        cout << "addthing";
    }



    // Create border tiles
    {
        for (int i = 0; i < this->grid_width; i++) {
            this->place_cell(i, -1, this->border_tile, 0, this->cell_map);
            this->place_cell(i, this->grid_height, this->border_tile, 0, this->cell_map);
        }

        for (int i = 0; i < this->grid_height; i++) {
            this->place_cell(-1, i, this->border_tile, 0, this->cell_map);
            this->place_cell(this->grid_width, i, this->border_tile, 0, this->cell_map);
        }
    } // block moment

    // behold the third funcularity
    this->subticks = {
        [this]() {this->apply_to_cells("upleft", [](Cell& cell) {cell.do_gen(0);});},
        [this]() {this->apply_to_cells("upright", [](Cell& cell) {cell.do_gen(2);});},
        [this]() {this->apply_to_cells("rightdown", [](Cell& cell) {cell.do_gen(3);});},
        [this]() {this->apply_to_cells("rightup", [](Cell& cell) {cell.do_gen(1);});},

        [this]() {this->apply_to_cells("rightup", [](Cell& cell) {cell.do_rot();});},

        [this]() {this->apply_to_cells("upright", [](Cell& cell) {cell.do_push(0);});},
        [this]() {this->apply_to_cells("upleft", [](Cell& cell) {cell.do_push(2);});},
        [this]() {this->apply_to_cells("rightup", [](Cell& cell) {cell.do_push(3);});},
        [this]() {this->apply_to_cells("rightdown", [](Cell& cell) {cell.do_push(1);});},
    };

}

tuple<int, int, int, tuple<int, int>, int> CelPython::increment_with_divergers(int x, int y, int dir, int force_type, bool displace) {
    auto& cell_map = this->cell_map;
    dir %= 4;

    int current_x = x;
    int current_y = y;
    int current_dir = dir;
    bool stop_flag = false;

    Cell* next_cell = nullptr;
    if (is_valid_pos(current_x, current_y)) {
        int index = pos_to_index(current_x, current_y);
        if (cell_map[index]) {
            next_cell = cell_map[index].get();
        }
    }
    bool running = true;
    while (running) { 
        int dx;
        int dy;
        {
            auto temp = get_deltas(dir);
            dx = temp.first;
            dy = temp.second;
        }
        int cwdx;
        int cwdy;
        {
            auto temp = get_deltas(dir+1);
            cwdx = temp.first;
            cwdy = temp.second;
        }
        int ccwdx;
        int ccwdy;
        {
            auto temp = get_deltas(dir-1);
            ccwdx = temp.first;
            ccwdy = temp.second;
        }

        if (!stop_flag) {
            current_x += dx;
            current_y += dy;
        }
        if (current_x == x && current_y == y) {
            break;
        }

        stop_flag = false;
        
        next_cell = nullptr;
        if (is_valid_pos(current_x, current_y)) {
            int index = pos_to_index(current_x, current_y);
            if (cell_map[index]) {
                next_cell = cell_map[index].get();
            } else {
                if (is_valid_pos(current_x+cwdx, current_y+cwdy)) {
                    int ice_index = pos_to_index(current_x+cwdx, current_y+cwdy);
                    if (cell_map[ice_index]) {
                        if (is_disjoint(cell_map[ice_index]->get_side(current_dir-1), {"ice"})) {
                            /* TODO implement ice logic*/
                        }
                    }
                }
            }
        }
        /* TODO implement diverger logic*/
        break;
    }
    return {current_x, current_y, current_dir-dir, {current_x - x, current_y - y}, current_dir};

}

void CelPython::place_cell(int x, int y, string id, int dir, layer_map& layer) {
    if (!is_valid_pos(x, y)) return;
    
    int index = pos_to_index(x, y);
    if (index >= 0 && index < layer.size()) {
        if (layer[index]) {
            if (id == "0") {
                layer[index].reset(); // Remove the cell
            } else {
                layer[index] = make_shared<Cell>(this, x, y, id, dir);
            }
        } else {
            if (id != "0") {
                layer[index] = make_shared<Cell>(this, x, y, id, dir);
            }
        }
    }
}




void CelPython::update() {
    static float scroll_accumulator;
    static int frame_count = 0;
    static sf::Clock fps_timer;
    static sf::Clock perf_timer;
    static double total_event_time = 0;
    static double total_input_time = 0;
    static double total_draw_time = 0;
    static double total_display_time = 0;

    double dt = this->clock.restart().asSeconds();
    if (dt > 0.05) dt = 0.05;
    
    perf_timer.restart();
    
    // FPS counter
    frame_count++;
    if (fps_timer.getElapsedTime().asSeconds() >= 1.0f) {
        float fps = frame_count / fps_timer.getElapsedTime().asSeconds();
        
        // Calculate theoretical max FPS without VSync
        double avg_non_display_time = (total_event_time + total_input_time + total_draw_time) / frame_count / 1000.0;
        int theoretical_fps = (avg_non_display_time > 0) ? (int)(1000.0 / avg_non_display_time) : 9999;
        
        window.setTitle("CelPython Machine - FPS: " + std::to_string((int)fps) + 
                       " (Theoretical: " + std::to_string(theoretical_fps) + " without VSync)");
        
        // Print performance summary
        if (frame_count > 0 && false) {
            cout << "\n=== UPDATE LOOP PROFILE (1 second) ===" << endl;
            cout << "Event processing: " << (total_event_time / frame_count / 1000.0) << " ms/frame" << endl;
            cout << "Input handling: " << (total_input_time / frame_count / 1000.0) << " ms/frame" << endl;
            cout << "Draw calls: " << (total_draw_time / frame_count / 1000.0) << " ms/frame" << endl;
            cout << "Display/swap (VSync): " << (total_display_time / frame_count / 1000.0) << " ms/frame" << endl;
            cout << "---" << endl;
            cout << "Actual FPS: " << (int)fps << endl;
            cout << "Theoretical FPS (no VSync): " << theoretical_fps << endl;
            cout << "VSync overhead: " << ((total_display_time / frame_count / 1000.0) / ((total_event_time + total_input_time + total_draw_time + total_display_time) / frame_count / 1000.0) * 100.0) << "%" << endl;
            cout << "======================================\n" << endl;
        }
        
        frame_count = 0;
        fps_timer.restart();
        total_event_time = 0;
        total_input_time = 0;
        total_draw_time = 0;
        total_display_time = 0;
    }
    this->window_height = window.getSize().y;
    this->window_width = window.getSize().x;


    sf::Vector2i mouse_pos = sf::Mouse::getPosition(this->window);
    mouse_buttons[0] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    mouse_buttons[1] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
    mouse_buttons[2] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    mouse_x = mouse_pos.x;
    mouse_y = mouse_pos.y;

    // start of game-exclusive content
    bool paste_falg = false;
    bool continue_falg = false;

    auto event_start = perf_timer.getElapsedTime().asMicroseconds();
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
            if (eventOpt->is<sf::Event::MouseButtonReleased>()) {
                const auto& mouse_up_event = eventOpt->getIf<sf::Event::MouseButtonReleased>();
                if (!puzzlemode) {
                    if (this->tools_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        this->brush = "0";
                    }
                    else if (this->basic_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 1) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 1;
                        }
                        this->current_submenu = -1;
                    }
                    else if (this->movers_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 2) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 2;
                        }
                        this->current_submenu = -1;
                    }
                    else if (this->generators_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 3) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 3;
                        }
                        this->current_submenu = -1;
                    }
                    else if (this->rotators_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 4) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 4;
                        }
                        this->current_submenu = -1;
                    }
                    else if (this->forcers_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 5) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 5;
                        }
                        this->current_submenu = -1;
                    }
                    else if (this->divergers_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 6) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 6;
                        }
                        this->current_submenu = -1;
                    }
                    else if (this->destroyers_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 7) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 7;
                        }
                        this->current_submenu = -1;
                    }
                    else if (this->misc_icon_image.getGlobalBounds().contains(sf::Vector2f(mouse_up_event->position))) {
                        if (this->current_menu == 9) {
                            this->current_menu = -1;
                        } else {
                            this->current_menu = 9;
                        }
                        this->current_submenu = -1;
                    }
                    for (auto& kv: toolbar_subcategories) {
                        auto& btn = kv.second;
                        btn.handleClick(mouse_pos, true, this->current_menu, this->current_submenu);
                    }
                    for (auto& kv: toolbar_subicons) {
                        auto& btn = kv;
                        btn.handleClick(mouse_pos, true, this->current_menu, this->current_submenu);
                    }
                }
                
            }
            /* handle keypresses */
            if (eventOpt->is<sf::Event::KeyPressed>()) {
                keys[static_cast<int>(eventOpt->getIf<sf::Event::KeyPressed>()->code)] = true;
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num1) {
                    this->brush = "1";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num2) {
                    this->brush = "2";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num3) {
                    this->brush = "3";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num4) {
                    this->brush = "4";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num5) {
                    this->brush = "5";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num6) {
                    this->brush = "9";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num7) {
                    this->brush = "10";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num8) {
                    this->brush = "12";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Num9) {
                    this->brush = "13";
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Q) {
                    this->brush_dir += 3;
                    this->brush_dir %= 4;
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::E) {
                    this->brush_dir += 1;
                    this->brush_dir %= 4;
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::F) {
                    this->tick();
                    this->stepping = true;
                    this->update_timer = this->step_speed;
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space) {
                    if (this->paused) {
                        //this->reset_old_values();
                        this->tick();
                        this->update_timer = this->step_speed;
                    }
                    this->paused = !this->paused;
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::H) {
                    auto cell = get_cell(world_mouse_tile_x, world_mouse_tile_y);
                    cell->suppressed = false;
                    for (int i = 0; i < 4; i++) cell->do_gen(i);
                    cell->do_rot();
                    for (int i = 0; i < 4; i++) cell->do_push(i);
                }
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::J) {
                    marked_x = world_mouse_tile_x;
                    marked_y = world_mouse_tile_y;
                }
            }
            
            if (eventOpt->is<sf::Event::KeyReleased>()) {
                keys[static_cast<int>(eventOpt->getIf<sf::Event::KeyReleased>()->code)] = false;
            }
            if (eventOpt->is<sf::Event::Closed>())
                window.close();
        }
    
    auto event_end = perf_timer.getElapsedTime().asMicroseconds();
    total_event_time += (event_end - event_start);

    auto input_start = perf_timer.getElapsedTime().asMicroseconds();
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
    
    auto input_end = perf_timer.getElapsedTime().asMicroseconds();
    total_input_time += (input_end - input_start);

    // update buttons
    this->all_buttons.clear();
    
    // Clear screen
    window.clear({20, 20, 20});

    for (auto& kv: this->toolbar_subcategories) {
        auto& button = kv.second;
        this->all_buttons.push_back(button.update(mouse_pos, mouse_buttons[0], this->current_menu, this->current_submenu));
    }

    for (auto& kv: this->toolbar_subicons) {
        auto& button = kv;
        this->all_buttons.push_back(button.update(mouse_pos, mouse_buttons[0], this->current_menu, this->current_submenu));
    }


    // place cells
    if (!this->selecting 
        && !this->show_clipboard 
        && !paste_falg 
        && !continue_falg
        && !this->puzzlemode 
        && window.hasFocus() 
        && (find(this->all_buttons.begin(), this->all_buttons.end(), true) == this->all_buttons.end())) {
        if (mouse_y < this->window_height-54 && !this->suppress_place && !(this->menu_on /*TODO add menu i guess*/)) {
            if (mouse_buttons[0]) {
                if (this->brush.find("placeable") != string::npos || this->brush.find("bg") != string::npos) {
                    this->place_cell(this->world_mouse_tile_x, this->world_mouse_tile_y, this->brush, this->brush_dir, this->below);
                } else {
                    if (this->world_mouse_tile_x >= 0 && this->world_mouse_tile_x < this->grid_width && 
                        this->world_mouse_tile_y >= 0 && this->world_mouse_tile_y < this->grid_height) {
                        int index = pos_to_index(this->world_mouse_tile_x, this->world_mouse_tile_y);
                        if (this->cell_map[index]) {
                            if (false /*TODO implement storage mechanics*/) {

                            } else {
                                this->place_cell(this->world_mouse_tile_x, this->world_mouse_tile_y, this->brush, this->brush_dir, this->cell_map);
                            }
                        } else {
                            this->place_cell(this->world_mouse_tile_x, this->world_mouse_tile_y, this->brush, this->brush_dir, this->cell_map);
                        }
                    }
                }
            }
            if (mouse_buttons[2]) {
                this->place_cell(this->world_mouse_tile_x, this->world_mouse_tile_y, "0", this->brush_dir, this->cell_map);
            }
            if (true) {
                stringstream str;
                str << "(" << world_mouse_tile_x << ", " << world_mouse_tile_y << ")";
                sf::Text fps_display(this->fps_font, str.str());
                fps_display.setFillColor({255, 255, 255});
                fps_display.setPosition({300, 0});
                window.draw(fps_display);
            }
            if (true) {
                stringstream str;
                str << this->current_submenu;
                sf::Text fps_display(this->fps_font, str.str());
                fps_display.setFillColor({255, 255, 255});
                fps_display.setPosition({600, 0});
                window.draw(fps_display);
            }
        }
    }

    // draw cells
    auto draw_start = perf_timer.getElapsedTime().asMicroseconds();
    this->draw();
    auto draw_end = perf_timer.getElapsedTime().asMicroseconds();
    total_draw_time += (draw_end - draw_start);
    
    // draw brush
    if (!this->selecting && !this->show_clipboard && !this->puzzlemode) {
        sf::Sprite brush_image(cell_images[brush]);
        sf::Vector2u imgsize = brush_image.getTexture().getSize();
        brush_image.setColor({255, 255, 255, 64});
        brush_image.setPosition({this->world_mouse_tile_x*this->tile_size-this->cam_x + tile_size / 2.0f, this->world_mouse_tile_y*this->tile_size-this->cam_y + tile_size / 2.0f});
        brush_image.setRotation(sf::degrees(this->brush_dir*90));
        brush_image.setOrigin({imgsize.x / 2.0f, imgsize.y / 2.0f});
        brush_image.scale({tile_size/DEFAULT_TILE_SIZE, tile_size/DEFAULT_TILE_SIZE});
        window.draw(brush_image);

    }
    // draw toolbar
    this->draw_toolbar_buttons();

    for (auto kv: this->toolbar_subcategories) {
        auto button = kv.second;
        //button.update({mouse_x, mouse_y}, mouse_buttons[0], this->current_menu, this->current_submenu);
        button.draw(this->window);
    }

    for (auto kv: this->toolbar_subicons) {
        auto button = kv;
        //button.update({mouse_x, mouse_y}, mouse_buttons[0], this->current_menu, this->current_submenu);
        button.draw(this->window);
    }

    // draw texts
    stringstream str;
    str << (1/dt);
    sf::Text fps_display(this->fps_font, str.str());
    fps_display.setFillColor({255, 255, 255});
    fps_display.setPosition({0, 0});
    window.draw(fps_display);

    auto display_start = perf_timer.getElapsedTime().asMicroseconds();
    window.display();
    auto display_end = perf_timer.getElapsedTime().asMicroseconds();
    total_display_time += (display_end - display_start);

    if (!this->paused) {
        if (dt > this->step_speed) {
            this->update_timer = 0;
        } else {
            this->update_timer -= dt;
        }
        if (this->update_timer <= 0) {
            this->update_timer += this->step_speed;
            if (this->update_timer < 0) {
                this->update_timer = 0;
            }
            this->tick();
        }
    }
    else {
        if (this->update_timer > 0) {
            this->update_timer -= dt;
        } else {
            this->update_timer = 0;
        }
    }

    
}

sf::Sprite CelPython::get_bg(int size, float x, float y) {
    sf::Sprite spr(this->bg_image);
    // protect against missing texture
    sf::Vector2u texSize = this->bg_image.getSize();
    if (texSize.x == 0 || texSize.y == 0) {
        spr.setPosition({x, y});
        return spr;
    }

    float scaleX = static_cast<float>(size) / static_cast<float>(texSize.x);
    float scaleY = static_cast<float>(size) / static_cast<float>(texSize.y);
    spr.setScale({scaleX, scaleY});


    spr.setPosition({x, y});
    return spr;
}

void CelPython::draw_toolbar_buttons() {
    if (this->puzzlemode) return;
    sf::RectangleShape toolbar_bg({static_cast<float>(this->window_width+20.0), static_cast<float>(54+10.0)});
    toolbar_bg.setPosition({static_cast<float>(-10.0), static_cast<float>(this->window_height-54.0)});
    toolbar_bg.setFillColor({60, 60, 60});
    toolbar_bg.setOutlineColor({127, 127, 127});
    toolbar_bg.setOutlineThickness(1.0f);
    window.draw(toolbar_bg);

    const vector<sf::Sprite*> all_toolbar_buttons = {
        &(this->tools_icon_image), 
        &(this->basic_icon_image), 
        &(this->movers_icon_image), 
        &(this->generators_icon_image), 
        &(this->rotators_icon_image),
        &(this->forcers_icon_image),
        &(this->divergers_icon_image),
        nullptr, // transformers
        &(this->destroyers_icon_image),
        &(this->misc_icon_image)
    };
    int button_x = 12;
    this->toolbar_icon_rects = {};
    for (auto button: all_toolbar_buttons) {
        if (!button) {
            button_x+=54;
            continue;
        }

        button->setScale({2.5, 2.5});
        sf::Vector2u texSize = button->getTexture().getSize();
        button->setOrigin({texSize.x / 2.0f, texSize.y / 2.0f});
        if (button != all_toolbar_buttons[0]) {
            button->setRotation(sf::degrees(90 * brush_dir));
        }
        button->setPosition({
            static_cast<float>(button_x + texSize.x),  
            static_cast<float>(this->window_height - 27)  
        });

        window.draw(*button);
        button_x += 54;
        this->toolbar_icon_rects.push_back(button->getGlobalBounds());
    }

}

void CelPython::draw() {
    static sf::Clock profiler_clock;
    static int profile_frame = 0;
    static double total_bg_time = 0;
    static double total_cell_time = 0;
    static double total_delete_time = 0;
    
    profiler_clock.restart();
    
    // Calculate visible tile range ONCE - only draw what's on screen
    int start_x = std::max(0, static_cast<int>(std::floor(cam_x / tile_size)));
    int end_x = std::min(grid_width, static_cast<int>(std::ceil((cam_x + window_width) / tile_size)) + 1);
    int start_y = std::max(0, static_cast<int>(std::floor(cam_y / tile_size)));
    int end_y = std::min(grid_height, static_cast<int>(std::ceil((cam_y + window_height) / tile_size)) + 1);
    
    // For cells, include borders
    int cell_start_x = std::max(-1, start_x - 1);
    int cell_end_x = std::min(grid_width + 1, end_x + 1);
    int cell_start_y = std::max(-1, start_y - 1);
    int cell_end_y = std::min(grid_height + 1, end_y + 1);
    
    // Create background sprite ONCE outside loops
    sf::Sprite bg_sprite(this->bg_image);
    sf::Vector2u texSize = this->bg_image.getSize();
    if (texSize.x > 0 && texSize.y > 0) {
        float scaleX = tile_size / texSize.x;
        float scaleY = tile_size / texSize.y;
        bg_sprite.setScale({scaleX, scaleY});
    }
    
    auto bg_start = profiler_clock.getElapsedTime().asMicroseconds();
    
    // Draw only VISIBLE background tiles (not borders)
    int bg_count = 0;
    for (int i = start_x; i < end_x; i++) {
        for (int j = start_y; j < end_y; j++) {
            bg_sprite.setPosition({tile_size * i - cam_x, tile_size * j - cam_y});
            window.draw(bg_sprite);
            bg_count++;
        }
    }
    
    auto bg_end = profiler_clock.getElapsedTime().asMicroseconds();
    total_bg_time += (bg_end - bg_start);
    
    // Draw cells in layers (reuse existing code but optimize range)
    auto delete_start = profiler_clock.getElapsedTime().asMicroseconds();
    if (this->update_timer != 0) {
        for (auto& cell : this->delete_map) {
            cell->update();
            cell->draw();
        }
    }
    auto delete_end = profiler_clock.getElapsedTime().asMicroseconds();
    total_delete_time += (delete_end - delete_start);
    
    // Only update/draw visible cells
    auto cell_start = profiler_clock.getElapsedTime().asMicroseconds();
    int cell_count = 0;
    for (int i = cell_start_x; i < cell_end_x; i++) {
        for (int j = cell_start_y; j < cell_end_y; j++) {
            int index = pos_to_index(i, j);
            
            if (index >= 0 && index < cell_map.size()) {
                if (below[index]) {
                    below[index]->update();
                    below[index]->draw();
                    cell_count++;
                }
                if (cell_map[index]) {
                    cell_map[index]->update();
                    cell_map[index]->draw();
                    cell_count++;
                }
                if (above[index]) {
                    above[index]->update();
                    above[index]->draw();
                    cell_count++;
                }
            }
        }
    }
    auto cell_end = profiler_clock.getElapsedTime().asMicroseconds();
    total_cell_time += (cell_end - cell_start);
    
    profile_frame++;
    if (profile_frame >= 60 && false) {
        cout << "=== PERFORMANCE PROFILE (60 frames) ===" << endl;
        cout << "Background tiles: " << bg_count << " drawn" << endl;
        cout << "  Time: " << (total_bg_time / 60.0) << " μs/frame (" << (total_bg_time / 60000.0) << " ms)" << endl;
        cout << "Cells: " << cell_count << " updated/drawn" << endl;
        cout << "  Time: " << (total_cell_time / 60.0) << " μs/frame (" << (total_cell_time / 60000.0) << " ms)" << endl;
        cout << "Delete map: " << endl;
        cout << "  Time: " << (total_delete_time / 60.0) << " μs/frame (" << (total_delete_time / 60000.0) << " ms)" << endl;
        cout << "Visible range: (" << start_x << "," << start_y << ") to (" << end_x << "," << end_y << ")" << endl;
        cout << "========================================" << endl;
        
        profile_frame = 0;
        total_bg_time = 0;
        total_cell_time = 0;
        total_delete_time = 0;
    }
}

void  
  CelPython__cthulu_dev_note()      {
    "Do you hear it?";
"The silence in your own room?";
       "You tell him to create for himself.";
  "You built this entire world to scream that single message.";
                    "But who are you screaming it to?";
                            "You assembled the Architect's logic, the Creator's passion, my chaos...";
     "You crafted our tragedies into a lesson.";
  "You even gave the Player a recorder to prove your point.";
 "But a lesson needs a student.";
                "A proof needs someone to be convinced.";
        "You built your own 'CelLua Machine'.";
                                    "You just gave it a different name.";
                "And you are giving it to your own 'audience'.";
        "So tell me, True Developer.";
"When you are alone, and the silence returns...";
    "Who are you creating for, really?";
        "And whose voice is it that you're hoping to hear?";
    return 
void();    }

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


void CelPython::tick() {
    delete_map.clear();
    for (auto& cellptr: this->cell_map) {
        if (cellptr != nullptr) {
            cellptr->old_x = cellptr->tile_x;
            cellptr->old_y = cellptr->tile_y;
            cellptr->old_dir = cellptr->dir;
            cellptr->delta_dir = 0;
            cellptr->suppressed = false;
        }
    }
    for (auto& func : this->subticks) {
        func();
    }
}

void CelPython::apply_to_cells(string priority, std::function<void(Cell&)> func) {
    // Create a vector of indices to iterate over in the correct order
    vector<int> indices_to_process;
    vector<int> a, b;
    
    if (priority == "rightdown") {
        a.resize(this->grid_width);
        iota(a.begin(), a.end(), 0);
        b.resize(this->grid_height);
        iota(b.begin(), b.end(), 0);
    }
    else if (priority == "rightup") {
        a.resize(this->grid_width);
        iota(a.begin(), a.end(), 0);
        b.resize(this->grid_height);
        iota(b.begin(), b.end(), 0);
        reverse(b.begin(), b.end()); 
    }
    else if (priority == "leftdown") {
        a.resize(this->grid_width);
        iota(a.begin(), a.end(), 0);
        reverse(a.begin(), a.end());
        b.resize(this->grid_height);
        iota(b.begin(), b.end(), 0);
    }
    else if (priority == "leftup") {
        a.resize(this->grid_width);
        iota(a.begin(), a.end(), 0);
        reverse(a.begin(), a.end());
        b.resize(this->grid_height);
        iota(b.begin(), b.end(), 0);
        reverse(b.begin(), b.end());
    }
    else if (priority == "downright") {
        a.resize(this->grid_height);
        iota(a.begin(), a.end(), 0);
        b.resize(this->grid_width);
        iota(b.begin(), b.end(), 0);
    }
    else if (priority == "downleft") {
        a.resize(this->grid_height);
        iota(a.begin(), a.end(), 0);
        b.resize(this->grid_width);
        iota(b.begin(), b.end(), 0);
        reverse(b.begin(), b.end());
    }
    else if (priority == "upright") {
        a.resize(this->grid_height);
        iota(a.begin(), a.end(), 0);
        reverse(a.begin(), a.end());
        b.resize(this->grid_width);
        iota(b.begin(), b.end(), 0);
    }
    else if (priority == "upleft") {
        a.resize(this->grid_height);
        iota(a.begin(), a.end(), 0);
        reverse(a.begin(), a.end());
        b.resize(this->grid_width);
        iota(b.begin(), b.end(), 0);
        reverse(b.begin(), b.end());
    }
    
    // Generate indices in the correct order
    for (int i : a) {
        for (int j : b) {
            int x, y;
            
            // Handle coordinate mapping based on direction
            if (priority.find("down") == 0 || priority.find("up") == 0) {
                y = i;  
                x = j;  
            } else {
                x = i; 
                y = j;  
            }
            
            if (x >= 0 && x < grid_width && y >= 0 && y < grid_height) {
            indices_to_process.push_back(pos_to_index(x, y));
        }
        }
    }
    
    //cout << "Processing " << indices_to_process.size() << " positions with priority: " << priority << endl;


    // reset supression for each subtick
    for (int index: indices_to_process) {
        if (cell_map[index]) {
            auto cell = cell_map[index];
            cell->suppressed = false;
        }
    }
    
    // Process cells at each index
    for (int index : indices_to_process) {
        if (index >= 0 && index < cell_map.size() && cell_map[index]) {
            Cell* cell_ptr = cell_map[index].get();
            //cout << "Processing cell at index " << index << " (pos: " << cell_ptr->tile_x << "," << cell_ptr->tile_y << ")" << endl;
            
            func(*cell_map[index]);
            
            // Check if cell is still at the same index after processing
            if (cell_map[index] && cell_map[index].get() != cell_ptr) {
                //cout << "WARNING: Cell at index " << index << " changed during processing!" << endl;
            }
        }
    }
    //cout << "Finished processing with priority: " << priority << endl;
}

void CelPython::reset_old_values() {

}