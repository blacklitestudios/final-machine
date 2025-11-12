#include "side.hpp"
#include <set>
#include <string>

using namespace std;

bool is_transformable(const set<string>& side) {
    if (side.contains("trash")) return false;
    if (side.contains("wall")) return false;
    return true;
}

bool is_destroyable(const set<string>& side) {
    if (side.contains("trash")) return false;
    if (side.contains("wall")) return false;
    return true;
}

bool is_unbreakable(const set<string>& side) {
    if (side.contains("trash")) return true;
    if (side.contains("wall")) return true;
    if (side.contains("storage")) return true;
    return false;
}