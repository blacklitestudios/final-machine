#ifndef CELL_HPP
#define CELL_HPP

#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include "cellinfo.hpp"

class CelPython;

class Cell {
    public: // ah yes very hidden data hiding
    CelPython *game;

    // std::map wants a default
    Cell();
    Cell(CelPython *game, int x, int y, string id, int dir, map<pair<int, int>, Cell> *layer = nullptr);

    int tile_x; // location of cell in the grid
    int tile_y; // location of cell in the grid
    int old_x; // used for animation
    int old_y; // used for animation
    int old_dir; // used for animation
    string id; // the id, usually a number, sometimes not
    string name; // the filename of the sprite file
    int dir; // where it is facing in the grid
    double actual_dir;
    map<int, sf::Sprite> img_cache;
    map<pair<int, int>, Cell> *layer;
    vector<int> chirality; // define into function
    int delta_dir;
    bool die_flag;
    sf::Texture image;
    sf::RectangleShape rect;

    bool frozen;
    bool shielded; // protected is a keyword :(

    string left;
    string right;
    string top;
    string bottom;

    string br;
    string bl;
    string tr;
    string tl;

    int hp;
    int generation;
    int weight;

    bool pushes;
    bool pulls;
    bool drills;
    bool cwgrabs;
    bool ccwgrabs;
    int gears;
    bool demolishes;
    bool nudges;
    vector<pair<int, int>> mirrors;
    bool bobs;

    bool suppressed;
    bool eat_left;
    bool eat_right;
    bool eat_top;
    bool eat_bottom;


    map<string, bool> tags;

    void update();
    void draw();



};

typedef map<pair<int, int>, Cell> layer_map;

#endif // CELL_HPP