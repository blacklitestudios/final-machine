#ifndef SIDE_HPP
#define SIDE_HPP

#include <set>
#include <string>

// Check if a side can be transformed (rotated, etc.)
bool is_transformable(const std::set<std::string>& side);

// Check if a side can be destroyed
bool is_destroyable(const std::set<std::string>& side);

// Check if a side is unbreakable
bool is_unbreakable(const std::set<std::string>& side);

#endif // SIDE_HPP
