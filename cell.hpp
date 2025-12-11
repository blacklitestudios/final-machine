#ifndef CELL_HPP
#define CELL_HPP

#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include "cellinfo.hpp"
#include "side.hpp"

struct PairHash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>{}(p.first) ^ (hash<int>{}(p.second) << 1);
    }
}; // the redeclaration

enum qual {NudgeSucceeded};

enum ForceReturn {failed, blocked, success};

class CelPython;

class Cell: public enable_shared_from_this<Cell> /* i have no idea what this does */ {
    public: // ah yes very hidden data hiding

    // std::map wants a default
    Cell();
    Cell(CelPython *game, int x, int y, string id, int dir);
    Cell(CelPython *game, int x, int y, string id, int dir, unordered_map<std::__1::pair<int, int>, Cell, PairHash> *layer);
    // No destructor needed - RAII handles cleanup automatically


    // i have no idea what these do either
    // Copy constructor
    Cell(const Cell& other);
    
    // Copy assignment operator  
    Cell& operator=(const Cell& other);
    
    // Move constructor and assignment (if needed)
    Cell(Cell&& other) noexcept;
    Cell& operator=(Cell&& other) noexcept;

    CelPython *game;
    int tile_x; // location of cell in the grid
    int tile_y; // location of cell in the grid
    int old_x; // used for animation
    int old_y; // used for animation
    int old_dir; // used for animation
    string id; // the id, usually a number, sometimes not
    string name; // the filename of the sprite file
    int dir; // where it is facing in the grid
    double actual_dir; // actual direction in degrees for transforming
    map<int, sf::Sprite> img_cache; // i don't even think this is used
    unordered_map<pair<int, int>, Cell, PairHash> *layer; // access current layer
    vector<int> chirality; // define into function
    int delta_dir; // for anims
    bool die_flag; // is this cell dead?

    string texture_id; // Store the texture ID to look up in game->cell_images
    optional<sf::Sprite> cached_sprite; // Cache sprite when needed

    bool frozen;
    bool shielded; // protected is a keyword :(
    bool locked = false;




    Cell* stored_cell;
    bool stores;

    bool suppressed;
    bool eat_left;
    bool eat_right;
    bool eat_top;
    bool eat_bottom;

    bool pushclamped;
    bool pullclamped;
    bool grabclamped;
    bool swapclamped;

    set<string> left;
    set<string> right;
    set<string> top;
    set<string> bottom;

    set<string> br;
    set<string> bl;
    set<string> tr;
    set<string> tl;

    int hp = 1;
    variant<string, int> generation = "normal";
    int weight = 1; // if this isn't a weight cell this doesn't matter
    bool rotatable = true;


    bool pushes = false;
    bool pulls = false;
    bool cwgrabs = false;
    bool ccwgrabs = false;
    bool drills = false;
    bool scissors = false;
    bool slices = false;

    int gears = 0;
    int minigears = 0;
    int skewgears = 0;
    bool demolishes;
    bool nudges = false;
    vector<pair<int, int>> mirrors = {};
    bool bobs = false;
    bool lifes = false;
    tuple<string, string, int> infects;

    map<string, bool> tags = {{"enemy", false}, {"ally", false}, {"neutral", false}, {"fiend", false}};

    map<string, any> extra_properties = {};
    map<string, any> dynamic_attrs = {};

    void update();
    void draw();

    void set_id(string id);

    set<qual> on_force(
        int dir, 
        Cell* origin, 
        string force_type, 
        int displacement, 
        map<string, bool> flags, // suppress is moved here
        int x,
        int y
    );

    void do_push(int dir);
    ForceReturn push(int dir, bool move, int hp=1, int force=0, int speed=1, bool bypass_bias = false, Cell* prev = nullptr, bool is_true=false);

    void do_gen(int dir);
    ForceReturn test_gen(int dir, int angle=0);
    shared_ptr<Cell> gen(int dir, shared_ptr<Cell> generated_cell);

    void do_rot();
    bool test_rot(int dir, int rot);
    void rot(int rot);
    ForceReturn nudge(int dir, bool move, int force=0, int hp=1, bool is_grab=false);

    double get_push_bias(int dir, double force, int times=-1, bool ignore_reverse=true);
    double get_forward_push_bias(int dir, double force=0.0, int times=-1);
    double get_reverse_push_bias(int dir, double force=0.0, int times=-1);
    
    // Helper function to safely move cells
    void move_to(int new_x, int new_y);

    sf::Sprite& getSprite();

    bool mexican_standoff(Cell* cell, bool destroy=true);
    void check_hp();

    set<string>& get_side(float dir2);
    bool is_unbreakable(int dir);
    bool is_pushable(int dir);
    bool is_eatable(int dir);
    bool is_crashable(int dir);
};

typedef vector<shared_ptr<Cell>> layer_map;

#endif // CELL_HPP