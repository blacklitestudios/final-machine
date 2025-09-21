#include <bits/stdc++.h>
#include "textbox.hpp"
#include <SFML/Graphics.hpp>

extern string ROOT;
using namespace std;

Textbox::Textbox(): font(ROOT + "nokiafc22.ttf"), txt_surface(font) {}