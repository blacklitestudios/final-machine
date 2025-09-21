#ifndef CELPYTHON_HPP
#define CELPYTHON_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <bits/stdc++.h> // screw clang for not automatically implementing this


#include "ui.hpp"
#include "slider.hpp"
#include "cell.hpp"

using namespace std;


class Textbox;
class MenuSubItem;
class MenuSubCategory;

extern std::map<std::string, sf::Texture> cell_images;
extern std::vector<std::pair<std::string, sf::Texture>> cell_images_raw;




class CelPython {
public:
    /* Window and rendering */
    int window_width = 800;
    int window_height = 600;
    sf::RenderWindow window;
    sf::Image icon;
    sf::Color BACKGROUND = sf::Color(31, 31, 31);
    float tile_size = 16.0f;
    int TOOLBAR_HEIGHT = 54;
    bool running = true;

    /* Time management */
    sf::Clock clock;
    float update_timer = 0.0f;

    /* UI elements */
    sf::Font fps_font;
    bool menu_on = false;
    int current_menu = 0;
    vector<sf::FloatRect> toolbar_icon_rects;
    vector<MenuSubItem> toolbar_subicons;
    map<int, MenuSubCategory> toolbar_subcategories;

    /* Game state */
    float scroll_speed = 250.0f;
    int border_tile = 41;
    float step_speed = 0.2f;
    int tpu = 1;
    int grid_width = 100;
    int grid_height = 100;
    bool paused = true;
    int tick_number = 0;

    /* Input handling */
    vector<bool> keys = vector<bool>(sf::Keyboard::KeyCount, false);
    bool mouse_buttons[3] = {false, false, false};
    int mouse_x = 0;
    int mouse_y = 0;
    float world_mouse_x = 0.0f;
    float world_mouse_y = 0.0f;
    int world_mouse_tile_x = 0;
    int world_mouse_tile_y = 0;

    /* Camera */
    float cam_x = 0.0f;
    float cam_y = 0.0f;

    /* Tools and brushes */
    string brush = "4";
    int brush_dir = 0;
    bool selecting = false;
    pair<int, int> select_start;
    pair<int, int> select_end;
    bool suppress_place = false;

    vector<pair<string, sf::Texture>> cell_images_raw;
    map<string, sf::Texture> cell_images;
    sf::Texture nonexistant;

    /* Clipboard */
    map<pair<int, int>, Cell> clipboard;
    map<pair<int, int>, Cell> clipboard_above;
    map<pair<int, int>, Cell> clipboard_below;
    bool show_clipboard = false;
    optional<pair<int, int>> clipboard_start;
    optional<pair<int, int>> clipboard_end;
    optional<pair<int, int>> clipboard_origin;

    /* Cell storage */
    map<pair<int, int>, Cell> cell_map;
    vector<pair<int, int>> delete_map;
    map<pair<int, int>, Cell> above;
    map<pair<int, int>, Cell> below;
    map<pair<int, int>, Cell> initial_cell_map;
    optional<map<pair<int, int>, Cell>> initial_above;
    optional<map<pair<int, int>, Cell>> initial_below;

    /* UI components */
    Button play_button;
    Button pause_button;
    Button step_button;
    Button initial_button;
    Button reset_button;
    Button menu_button;
    Textbox width_box;
    Textbox height_box;
    Slider tickspeed_slider;
    Slider tpu_slider;

    /* Graphics resources */
    sf::Texture bg_image;
    map<string, sf::Texture> bg_cache;
    sf::Texture tools_icon_texture;
    sf::Sprite tools_icon_image;
    sf::Texture basic_icon_texture;
    sf::Sprite basic_icon_image;
    sf::Texture movers_icon_texture;
    sf::Sprite movers_icon_image;
    sf::Texture generators_icon_texture;
    sf::Sprite generators_icon_image;
    sf::Texture rotators_icon_texture;
    sf::Sprite rotators_icon_image;
    sf::Texture forcers_icon_texture;
    sf::Sprite forcers_icon_image;
    sf::Texture divergers_icon_texture;
    sf::Sprite divergers_icon_image;
    sf::Texture destroyers_icon_texture;
    sf::Sprite destroyers_icon_image;
    sf::Texture misc_icon_texture;
    sf::Sprite misc_icon_image;
    sf::Texture menu_bg;
    sf::Texture placeable_overlay;
    sf::Texture freeze_image;
    sf::Texture protect_image;
    

    /* Audio */
    sf::SoundBuffer beep_buffer;
    sf::Sound beep;
    sf::SoundBuffer destroy_sound_buffer;
    sf::Sound destroy_sound;
    sf::SoundBuffer move_sound_buffer;
    sf::Sound move_sound;
    sf::SoundBuffer rotate_sound_buffer;
    sf::Sound rotate_sound;
    sf::SoundBuffer gear_sound_buffer;
    sf::Sound gear_sound;
    sf::Music music;

    /* Game logic */
    const vector<string> border_tiles;
    optional<pair<int, int>> selected_cell;
    string title;
    string subtitle;
    string result;
    bool puzzlemode = false;
    bool builtin_puzzle = false;

    /* UI groups */
    vector<Button> buttonz;
    vector<Button> puzzles_group;
    vector<Button> topleft_button_group;

    /* Tags and metadata */
    map<string, int> initial_tags;
    map<string, int> tags;

    CelPython();
    void play();
    // string resource_path(string relative_path); // not needed, this is c++
    void load_image(string path);
    sf::Font nokia(int size);
    sf::Sprite get_bg(int size, float x, float y);
    void place_cell(int x, int y, string id, int dir, layer_map& layer);
    Cell get_cell(int x, int y);
    void apply_to_cells(string priority, function<void(Cell)>);
    void victory();
    void failure();
    void tick();
    void draw();
    layer_map copy_map(layer_map cm);
    void set_initial();
    void reset();
    void trash();
    void reset_old_values();
    void delete_selected();
    void copy_selected();
    void scroll_up(int x, int y);
    void scroll_down(int x, int y);
    void get_keys();
    bool is_key_pressed(sf::Keyboard::Key keycode) {return keys[static_cast<int>(keycode)];};
    void update();
    void save_map();
    void decode_K3(string code);
    Cell get_cell(int x, int y, int z);
    string encode_cell(int x, int y, int l);
    Cell decode_cell(string code);
    string encode_raw_cell(Cell cell, bool is_below);
    string encode_data(int data);
    string encode_data(string data);
    string encode_data(bool data);
    variant<int, string, bool> decode_data(string code);
    void load_map();
    //void play();




     
};

#endif // CELPYTHON_HPP