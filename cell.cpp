#include <memory>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>
#include "cell.hpp"
#include "game.hpp"
#include "cellinfo.hpp"
#include "side.hpp"



extern int DEFAULT_TILE_SIZE;



using namespace std;

// Static sets to avoid temporary allocations during heap corruption
static const set<string> GENERATOR_SET = {"generator"};
static const set<string> STORAGE_SET = {"storage"};
static const unordered_set<string> TRASH_PHANTOM = {"trash", "phantom"};
static const unordered_set<string> WALL_SET = {"wall"};
static const unordered_set<string> ENEMY_SET = {"enemy"};
static const unordered_set<string> UNPUSHABLE_UNDIRECTIONAL = {"unpushable", "undirectional"};
static const unordered_set<string> REPULSE_SET = {"repulse"};

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline int positive_modulo(int i, int n) {
    return (i % n + n) % n;
}

// oh look an ai generated code
// how quaint
template <typename Map>
typename Map::mapped_type map_get(
    const Map& m, 
    const typename Map::key_type& key, 
    const typename Map::mapped_type& default_value) 
{
    // Search for the key
    auto it = m.find(key);

    // If found, return the value from the map
    if (it != m.end()) {
        return it->second;
    } 
    // If not found, return the default value
    else {
        return default_value;
    }
}



// Add to cell.cpp
Cell::Cell(const Cell& other) 
    : game(other.game),
      tile_x(other.tile_x),
      tile_y(other.tile_y),
      old_x(other.old_x),
      old_y(other.old_y),
      old_dir(other.old_dir),
      id(other.id),
      name(other.name),
      dir(other.dir),
      actual_dir(other.actual_dir),
      img_cache(), // Don't copy cache - let it rebuild
      layer(nullptr), // Don't copy layer pointer
      chirality(other.chirality),
      delta_dir(other.delta_dir),
      die_flag(other.die_flag),
      texture_id(other.texture_id),
      cached_sprite(), // Don't copy sprite - let it rebuild
      frozen(other.frozen),
      shielded(other.shielded),
      hp(other.hp),
      generation(other.generation),
      weight(other.weight),
      pushes(other.pushes),
      pulls(other.pulls),
      drills(other.drills),
      cwgrabs(other.cwgrabs),
      ccwgrabs(other.ccwgrabs),
      gears(other.gears),
      demolishes(other.demolishes),
      nudges(other.nudges),
      mirrors(other.mirrors),
      bobs(other.bobs),
      suppressed(other.suppressed),
      eat_left(other.eat_left),
      eat_right(other.eat_right),
      eat_top(other.eat_top),
      eat_bottom(other.eat_bottom),
      tags(other.tags)
{
    // Don't copy cached resources - they'll be rebuilt as needed
}

// New constructor for vector-based storage
Cell::Cell(CelPython *game, int x, int y, string id, int dir) 
    : game(game),
      tile_x(x),
      tile_y(y),
      old_x(x),
      old_y(y),
      old_dir(dir),
      id(id),
      name(""),
      dir(dir),
      actual_dir(dir * -90.0f),
      img_cache({}),
      layer(nullptr),
      chirality({-1, 0, 1, 2}),
      delta_dir(0),
      die_flag(false),
      texture_id(""), 
      cached_sprite(), 
      frozen(false),
      shielded(false),
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
    auto it = cell_names.find(id);
    this->name = (it != cell_names.end() ? it->second : "nonexistant");

    this->texture_id = id;
    this->set_id(this->id);
}

sf::Sprite& Cell::getSprite() {
    if (!cached_sprite.has_value()) {
        auto it = this->game->cell_images.find(texture_id);
        if (it != this->game->cell_images.end()) {
            cached_sprite = sf::Sprite(it->second);
        } else {
            cached_sprite = sf::Sprite(this->game->nonexistant);
        }
    }
    return cached_sprite.value();
}

void Cell::update() {
    if (this->texture_id.empty()) {
        return;
    }

    sf::Sprite& sprite = this->getSprite();
    double factor = (this->game->step_speed != 0) ? (this->game->update_timer/this->game->step_speed) : (1);
    this->actual_dir = int(lerp(this->dir*90, (this->dir-this->delta_dir)*90, factor));
    sf::Vector2u imgsize = sprite.getTexture().getSize();
    
    sprite.setRotation(sf::degrees(this->actual_dir));
    sprite.setScale({this->game->tile_size / DEFAULT_TILE_SIZE, this->game->tile_size / DEFAULT_TILE_SIZE});
    sprite.setOrigin({imgsize.x/2.0f, imgsize.y/2.0f});

    float world_x = lerp(float(this->tile_x * this->game->tile_size), 
                         float(this->old_x * this->game->tile_size), 
                         factor) + this->game->tile_size / 2.0f;
    
    float world_y = lerp(float(this->tile_y * this->game->tile_size), 
                         float(this->old_y * this->game->tile_size), 
                         factor) + this->game->tile_size / 2.0f;
    float screen_x = world_x - this->game->cam_x;
    float screen_y = world_y - this->game->cam_y;
    
    sprite.setPosition({screen_x, screen_y});
}

void Cell::draw() {
    sf::Sprite& sprite = this->getSprite();
    if (!this->texture_id.empty()) {
        this->game->window.draw(sprite);
    }
}

void Cell::set_id(string id) {
    this->id = id;
    this->texture_id = id;
    this->name = cell_names[id];

    this->left = {"pushable"};
    this->right = {"pushable"};
    this->top = {"pushable"};
    this->bottom = {"pushable"};

    this->br = this->bl = this->tr = this->tl = {"pushable"};

    this->pushes = false;

    // TODO add the rest of the things

    switch (stoi(id)) {
        case 1: this->right = this->left = this->top = this->bottom = {"wall"}; break;
        case 2: this->pushes = true; this->chirality = {0}; break;
        case 3: this->right = {"generator"}; this->chirality = {0}; break;
        case 4: break;
        case 5: this->top = this->bottom = {"undirectional"}; break;
        case 9: this->right = this->left = this->top = this->bottom = {"cwrotator"}; break;
        case 10: this->right = this->left = this->top = this->bottom = {"ccwrotator"}; break;
        case 11: this->right = this->left = this->top = this->bottom = {"180rotator"}; break;
        case 12: this->right = this->left = this->top = this->bottom = {"trash"}; break;
        case 13: this->right = this->left = this->top = this->bottom = {"enemy"}; break;
        case 41: this->right = this->left = this->top = this->bottom = {"wall"}; break;

    }
}

ForceReturn Cell::push(int dir, bool move, int hp, int force, int speed, bool bypass_bias, Cell* prev, bool is_true) {
    //cout << "pushing at pos " << tile_x << ", " << tile_y << endl;
    auto& cell_map = this->game->cell_map; // Use reference to avoid copy

    if (speed == 1 || !move) {
        int bias = force;
        bool trash_flag = false;
        bool enemy_flag = false;

        bool fail = false;
        if (!move) {
            bias += 0;
        }
        if (this->get_side(dir+2).contains(string("repulse"))) {
            bias += 1;
        }

        auto [fx, fy, dfdir, _, fdir] = this->game->increment_with_divergers(this->tile_x, this->tile_y, dir);

        if (this->dir == dir && this->pushes) this->suppressed = true;

        // Check if there's a cell to push
        if (this->game->does_cell_exist(fx, fy)) {
            Cell* cell = this->game->get_cell(fx, fy).get();
            if (!cell->is_pushable(fdir)) {
                cell->on_force(fdir, this, "push", dfdir, {}, this->tile_x, this->tile_y);
                fail = true;
            }
            if (cell->is_eatable(fdir)) {
                trash_flag = true;
                fail = false;
            }
            if (cell->is_crashable(dir) && !cell->shielded) {
                enemy_flag = true;
                fail = false;
            }
            if (map_get(cell->extra_properties, "stallenemy", any{}).has_value()) {
                fail = true;
            }
        }
        int x = fx;
        int y = fy;
        //shared_ptr<Cell> cell = shared_from_this();


        bias = (bypass_bias ? INFINITY : this->get_push_bias(dir, bias));

        if (bias <= 0) {
            fail = true;
        }

        if (is_true) {
            fail = false;
            trash_flag = false;
        }

        if (move && prev && !fail) {
            this->on_force(dir, prev, "push", dfdir, {}, this->tile_x, this->tile_y);
        }

        if (this->game->does_cell_exist(fx, fy)) {
            Cell* fcell = this->game->get_cell(fx, fy).get();
            if (is_true 
                || (!trash_flag 
                && !enemy_flag 
                && !fcell->pushclamped 
                && !fail 
                && (is_disjoint(fcell->get_side(fdir), STORAGE_SET)))) {
                int original_dir = this->dir;
                this->dir += dfdir;
                bool result = fcell->push(fdir, true, 1, INFINITY, speed, true, (move ? this : nullptr), is_true);
                this->dir = original_dir;
                if (!result) {
                    return is_true ? ForceReturn::success : ForceReturn::failed;
                }
            }
        }
        
        // Now try to move this cell
        if (move) {
            //cout << "nudging at pos" << tile_x << ", " << tile_y << endl;
            if (!this->nudge(dir, true)) return ForceReturn::blocked;
        }
        
        if (fail) return ForceReturn::blocked;
    }
    return ForceReturn::success;
} 

void Cell::do_push(int dir) {
    auto keep_alive = shared_from_this();
    if (this->suppressed || this->frozen || this->dir != dir) return;
    if (this->tile_x == this->game->marked_x && this->tile_y == this->game->marked_y && dir==3 && this->pushes) {
        cout << endl;
    }
    if (this->pushes && this->dir==dir) {
        if (!this->push(dir, true))
        ;
        this->suppressed = true;
    }
}

void Cell::do_gen(int dir) {
    // Ultimate safety validation
    if (!this->game) return;
    
    // Validate this cell exists and hasn't been moved/deleted

    
    if (this->frozen) return;

    
    static const set<string> GENERATOR_SET{"generator"};
    
    if (!is_disjoint(this->get_side(dir), GENERATOR_SET)) {
        // Additional safety check before calling test_gen
        if (this->game && this->game->does_cell_exist(this->tile_x, this->tile_y)) {
            if (this->tile_x == this->game->marked_x && this->tile_y == this->game->marked_y) {
                cout << endl;
            }
            this->test_gen(dir, 0);
        }
    }
}


ForceReturn Cell::test_gen(int dir, int angle) {
    auto cell_map = this->game->cell_map;
    auto [dx, dy] = get<3>(this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo((dir-angle+2), 4)));
    auto [odx, ody] = get<3>(this->game->increment_with_divergers(this->tile_x, this->tile_y, dir));
    
    Cell* behind_cell;
    if (this->game->does_cell_exist(this->tile_x + dx, this->tile_y+dy)) {
        behind_cell = this->game->get_cell(this->tile_x + dx, this->tile_y+dy).get();
    } else {
        if (!this->get_side(dir).contains("memorygen")) {
            return failed;
        }
        behind_cell = this->stored_cell;
    }
    
    // Create as shared_ptr from the beginning
    auto generated_cell = make_shared<Cell>(this->game, this->tile_x+odx, this->tile_y+ody, 
                                           behind_cell->id, positive_modulo((behind_cell->dir+angle),4));
    generated_cell->suppressed = false;
    
    if (this->game->does_cell_exist(this->tile_x+dx, this->tile_x+dy)) {
        // Handle existing cell case
    }
    
    auto temp = this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo((dir-angle),4));
    if (this->get_side(dir).contains("memorygen")) {}
    
    this->gen(dir, generated_cell);  // Pass the shared_ptr
    return success;
}

shared_ptr<Cell> Cell::gen(int dir, shared_ptr<Cell> generated_cell) {
    auto& cell_map = this->game->cell_map;
    auto out_incr = this->game->increment_with_divergers(tile_x, tile_y, positive_modulo(dir, 4));
    auto [dx, dy] = get<3>(out_incr);
    auto out_dir = get<4>(out_incr);
    auto out_ddir = get<2>(out_incr);
    if (this->game->marked_x == this->tile_x && this->game->marked_y == this->tile_y) {
        cout << endl;
    }
    
    if (game->does_cell_exist(get<0>(out_incr), get<1>(out_incr))) {
        auto front_cell = game->get_cell(get<0>(out_incr), get<1>(out_incr));
        if (front_cell->is_pushable(out_dir)) {
            if (!front_cell->push(out_dir, true, 1, 1, 1, false, generated_cell.get())) {
                return nullptr;
            }
        }
    }
    
    if (!game->does_cell_exist(tile_x+dx, tile_y+dy)) {
        generated_cell->old_x = tile_x;
        generated_cell->old_y = tile_y;
        generated_cell->tile_x = tile_x+dx;
        generated_cell->tile_y = tile_y+dy;
        generated_cell->dir += out_ddir;
        generated_cell->dir = positive_modulo(generated_cell->dir, 4);
        
        if (!generated_cell->id.empty()) {
            cell_map[game->pos_to_index(tile_x+dx, tile_y+dy)] = generated_cell;
            cout << generated_cell->id << endl;
        }
    }
    
    return generated_cell;  // Return the same shared_ptr
}

void Cell::do_rot() {
    if (this->frozen) return;
    for (int i = 0; i < 4; i++) {
        if (this->get_side(i).contains("cwrotator")) this->test_rot(i, 1);
        if (this->get_side(i).contains("180rotator")) this->test_rot(i, 2);
        if (this->get_side(i).contains("ccwrotator")) this->test_rot(i, -1);
    }
}

bool Cell::test_rot(int dir, int rot) {
    auto cell_map = this->game->cell_map;
    auto [dx, dy] = get_deltas(dir);
    Cell* target_cell = nullptr;
    if (this->game->does_cell_exist(this->tile_x+dx, this->tile_y+dy)) {
        target_cell = this->game->get_cell(this->tile_x+dx, this->tile_y+dy).get();
    } else {
        return false;
    }
    if (target_cell->get_side(dir+2).contains("wall")) return false;
    target_cell->rot(rot);
    return true;
} 

void Cell::rot(int rot) {
    if (this->rotatable && !this->locked) {
        this->dir += rot;
        this->delta_dir += rot;
        this->dir = positive_modulo(this->dir, 4);
        //if (rot % 4) ;

    }
}

bool Cell::mexican_standoff(Cell* cell, bool destroy) {
    if (this->shielded || cell->shielded) return true;
    double dec_hp = min(this->hp, cell->hp);
    if (destroy) {
        this->hp -= dec_hp;
        cell->hp -= dec_hp;

        this->check_hp();
        cell->check_hp();
        // play destroy sound
    }
    if (this->hp <= (destroy ? 0 : cell->hp)) return false;
    return true;
}

void Cell::check_hp() {
    if (this->hp == 1 && this->id == "24") this->set_id("13");
    if (this->hp == 2 && this->id == "13") this->set_id("24");
    if (this->hp == 0) {
        this->game->get_cell(this->tile_x, this->tile_y).reset();
        
    }
}

ForceReturn Cell::nudge(int dir, bool move, int force, int hp, bool is_grab) {
    auto& cell_map = this->game->cell_map;
    auto keep_alive = cell_map[this->game->pos_to_index(this->tile_x, this->tile_y)]; // don't kill me yet, i'm still useful...
    auto [new_x, new_y, ddir, a, b] = this->game->increment_with_divergers(this->tile_x, this->tile_y, dir, 0, true);
    shared_ptr<tuple<int, int, int, tuple<int, int>, int>> killer_cell = nullptr;
    Cell* actual_killer_cell = nullptr;
    bool suicide_flag = false;
    bool enemy_flag = false;
    
    if (this->game->does_cell_exist(new_x, new_y)) {
        auto cell = this->game->get_cell(new_x, new_y).get();
        if (cell->is_eatable(b)) {
            suicide_flag = true;
            auto ofret = cell->on_force(b, this, "nudge", 0, {}, this->tile_x, this->tile_y);
        }

        if ((cell->get_side(dir+2+ddir).contains("enemy") || this->get_side(dir).contains("enemy")) && !cell->shielded && !this->shielded) {
            //this->game->play_destroy_flag = true;
            if (is_destroyable(cell->get_side((dir+2+ddir)))) {
                if (!this->mexican_standoff(cell, move)) {
                    this->tile_x = new_x;
                    this->tile_y = new_y;
                    this->rot(ddir);
                }
            }
            if (this->hp == 0) {
                this->tile_x = new_x;
                this->tile_y = new_y;
                this->rot(ddir);
                suicide_flag = true;
                killer_cell = make_shared<tuple<int, int, int, tuple<int, int>, int>>(new_x, new_y, ddir, a, b);
                enemy_flag = true;

            }
        }
        //cout << this->game->get_cell(new_x, new_y)->id << endl;
        else if (this->game->get_cell(new_x, new_y)->get_side((dir+2+ddir)).contains("trash")) {

            suicide_flag = true;
            killer_cell = make_shared<tuple<int, int, int, tuple<int, int>, int>>(new_x, new_y, ddir, a, b);
            actual_killer_cell = this->game->get_cell(new_x, new_y).get();
        }
        else if (cell->cwgrabs && cell->ccwgrabs && is_grab && cell->dir == dir && move)  {
            //cell->grabs(....)
        }
    }


    if (!suicide_flag) {
        if (this->game->is_valid_pos(new_x, new_y)) {
            int index = this->game->pos_to_index(new_x, new_y);
            if (this->game->cell_map[index]) {
                auto front_cell = this->game->get_cell(new_x, new_y).get();
                this->game->cell_map[this->game->pos_to_index(this->tile_x, this->tile_y)] = shared_from_this();
                auto ofret = front_cell->on_force(b, this, "nudge", 0, {}, this->tile_x, this->tile_y);
                if (this->extra_properties.find("lifemissile") != this->extra_properties.end()) {
                    this->set_id("149");
                }
                return ofret.contains(NudgeSucceeded) ? success : failed;
            }
        }
    }

    if (move) {
        // Use safe helper function to avoid map corruption
        cell_map[this->game->pos_to_index(this->tile_x, this->tile_y)].reset();
        
        if (positive_modulo(this->dir, 4) == positive_modulo(dir, 4)) {
            this->suppressed = true;
        }

        if (suicide_flag) {
            if (!enemy_flag && killer_cell) {
                auto ofret = this->game->get_cell(get<0>(*killer_cell), get<1>(*killer_cell))->on_force(b, this, "nudge", ddir, {}, this->tile_x, this->tile_y);
                this->game->get_cell(this->tile_x, this->tile_y).reset();
            }
            this->tile_x = get<0>(*killer_cell);
            this->tile_y = get<1>(*killer_cell);
            this->rot(ddir);
            if (!enemy_flag && actual_killer_cell) {
                if (actual_killer_cell->get_side(dir+ddir+2).contains("trash")) {
                    this->game->delete_map.push_back(shared_from_this());
                }
            }
        } else {
            this->game->get_cell(new_x, new_y) = shared_from_this();
            this->tile_x = new_x;
            this->tile_y = new_y;
            this->rot(ddir);
            //cell_map[this->game->pos_to_index(new_x, new_y)] = keep_alive;
        }


        
    }
    return success; 
}

set<qual> Cell::on_force(
        int dir, 
        Cell* origin, 
        string force_type, 
        int displacement, 
        map<string, bool> flags, // suppress is moved here
        int x,
        int y
    ) {return {};}

double Cell::get_push_bias(int dir, double force, int times, bool ignore_reverse) {
    auto cell_map = this->game->cell_map;
    if (times == 0) return force;
    double bias = force;
    auto fore_incr = this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo(dir, 4), 0, true);
    auto back_incr = this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo(dir+2, 4), 0, true);

    if (this->pushes && positive_modulo(this->dir, 4) == positive_modulo(dir, 4)) bias++;
    if (this->pushes && positive_modulo(this->dir, 4) == positive_modulo(dir+2, 4)) bias--;
    if (this->get_side(dir+2).contains("repulse")) bias--;
    // handle weights
    if (bias == -INFINITY) return -INFINITY;
    if (this->is_eatable(dir)) return bias;
    if (this->game->does_cell_exist(get<0>(fore_incr), get<1>(fore_incr))) {
        bias = this->game->get_cell(get<0>(fore_incr), get<1>(fore_incr))->get_forward_push_bias(positive_modulo(get<4>(fore_incr), 4), bias, times-1);
    } 
    if (bias <= 0) return bias;
    if (this->game->does_cell_exist(get<0>(back_incr), get<1>(back_incr)) && !ignore_reverse) {
        bias = this->game->get_cell(get<0>(back_incr), get<1>(back_incr))->get_reverse_push_bias(positive_modulo(get<4>(back_incr)+2, 4), bias, times-1);
    } 
    return bias;
}

double Cell::get_forward_push_bias(int dir, double force, int times) {
    auto cell_map = this->game->cell_map;
    if (times == 0) return force;
    double bias = force;
    auto fore_incr = this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo(dir, 4), 0, true);
    auto back_incr = this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo(dir+2, 4), 0, true);

    if (this->pushes && positive_modulo(this->dir, 4) == positive_modulo(dir, 4)) bias++;
    if (this->pushes && positive_modulo(this->dir, 4) == positive_modulo(dir+2, 4)) bias--;
    if (this->get_side(dir+2).contains("repulse")) bias--;
    // handle weights
    if (bias == -INFINITY) return -INFINITY;

    if (this->is_eatable(dir)) return bias;
    if (this->game->does_cell_exist(get<0>(fore_incr), get<1>(fore_incr))) {
        bias = this->game->get_cell(get<0>(fore_incr), get<1>(fore_incr))->get_forward_push_bias(positive_modulo(get<4>(fore_incr), 4), bias, times-1);
    } 

    return bias;
}

double Cell::get_reverse_push_bias(int dir, double force, int times) {
    auto cell_map = this->game->cell_map;
    if (times == 0) return force;
    double bias = force;
    //auto fore_incr = this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo(dir, 4), 0, true);
    auto back_incr = this->game->increment_with_divergers(this->tile_x, this->tile_y, positive_modulo(dir+2, 4), 0, true);

    if (this->pushes && positive_modulo(this->dir, 4) == positive_modulo(dir, 4)) bias++;
    if (this->pushes && positive_modulo(this->dir, 4) == positive_modulo(dir+2, 4)) bias--;
    if (this->get_side(dir+2).contains("repulse")) bias--;
    // handle weights
    if (bias == -INFINITY) return -INFINITY;
    if (this->is_eatable(dir)) return bias;

    if (this->game->does_cell_exist(get<0>(back_incr), get<1>(back_incr))) {
        bias = this->game->get_cell(get<0>(back_incr), get<1>(back_incr))->get_reverse_push_bias(positive_modulo(get<4>(back_incr)+2, 4), bias, times-1);
    } 
    return bias;
}

bool Cell::is_unbreakable(int dir) {
    if (!is_disjoint(this->get_side(dir+2), {"wall"})) return true;
    if (this->is_eatable(dir)) return true;
    return false;
}
bool Cell::is_eatable(int dir) {
    if (!is_disjoint(this->get_side(dir+2), {"trash", "phantom"})) return true;
    return false;
}
bool Cell::is_crashable(int dir) {
    if (!is_disjoint(this->get_side(dir+2), {"enemy"})) return true;
    return false;
}
bool Cell::is_pushable(int dir) {
    if (this->is_unbreakable(dir)) return false;
    if (!is_disjoint(this->get_side(dir), {"unpushable", "undirectional"})) return false;
    if (this->is_crashable(dir)) return false;
    if (this->pushclamped) return false;
    return true;
}



set<string>& Cell::get_side(float dir2) {
    static set<string> empty_set;
    
    int normalized_dir = positive_modulo(-this->dir*2 + static_cast<int>(dir2*2), 8);
    
    switch (normalized_dir) {
        case 0: return this->right;
        case 1: return this->br;
        case 2: return this->bottom;
        case 3: return this->bl;
        case 4: return this->left;
        case 5: return this->tl;
        case 6: return this->top;
        case 7: return this->tr;
        default: return empty_set;
    }
}