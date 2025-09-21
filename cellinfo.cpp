#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace std;

std::map<std::string, std::string> cell_names = {
    {"placeable", "placeable"},
    {"0", "eraser"},
    {"1", "wall"},
    {"2", "mover"},
    {"3", "generator"},
    {"4", "push"},
    {"41", "ghost"}
};

// Definitions for global textures (needed for linker)

std::map<std::string, sf::Texture> cell_images;
std::vector<std::pair<std::string, sf::Texture>> cell_images_raw;

vector<vector<vector<string>>> cell_cats_new = {
    {}, // Tools
    {
        {"1", "41"}, // Walls
        {"4"}, // Pushables
    }, // Basic
    {
        {"2"},
    }, // Movers
    {
        {"3"},
    }, // Generators
    {

    }, // Rotators 
    {

    }, // Forcers
    {

    }, // Destroyers
    {

    }, // Transformers
    {

    }, // Misc
};