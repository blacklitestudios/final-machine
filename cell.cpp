#include <bits/stdc++.h>
#include "cell.hpp"
#include "game.hpp"
#include "cellinfo.hpp"
using namespace std;

Cell::Cell()
    : game(nullptr),
      tile_x(0),
      tile_y(0),
      old_x(0),
      old_y(0),
      old_dir(0),
      id(""),
      name(""),
      dir(0),
      actual_dir(0.0),
      img_cache(),
      layer(nullptr),
      chirality(),
      delta_dir(0),
      die_flag(false),
      image(), // will be set below
      rect(),
      frozen(false),
      shielded(false),
      left(""),
      right(""),
      top(""),
      bottom(""),
      br(""),
      bl(""),
      tr(""),
      tl(""),
      hp(0),
      generation(0),
      weight(0),
      pushes(false),
      pulls(false),
      drills(false),
      cwgrabs(false),
      ccwgrabs(false),
      gears(0),
      demolishes(false),
      nudges(false),
      mirrors(),
      bobs(false),
      suppressed(false),
      eat_left(false),
      eat_right(false),
      eat_top(false),
      eat_bottom(false),
      tags()
{
    image = sf::Texture();
}

Cell::Cell(CelPython *game, int x, int y, string id, int dir, map<pair<int, int>, Cell> *layer) {
    this->game = game;

    this->tile_x = x;
    this->tile_y = y;
    this->dir = dir;
    this->id = id;

    this->old_x = x;
    this->old_y = y;
    this->old_dir = dir;

    {
        auto it = cell_names.find(id);
        this->name = (it != cell_names.end() ? (*it).second : "nonexistant");
    }
    this->dir = dir;
    this->actual_dir = dir*-90;
    this->img_cache = {};

    if (layer != nullptr) {
        this->layer = layer;
    } else {
        this->layer = &(this->game->cell_map);
    }

    this->chirality = {-1, 0, 1, 2};
    this->delta_dir = 0;
    this->die_flag = false;

    // image variables
    {
        auto it = cell_images.find(id);
        this->image = (it != this->game->cell_images.end() ? (*it).second : this->game->nonexistant);
    }

}

void Cell::draw() {
    sf::Sprite true_image(this->image);
    true_image.setRotation(sf::degrees(this->actual_dir));
    this->game->window.draw(true_image);
}