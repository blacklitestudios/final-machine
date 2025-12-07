#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>

#include "game.hpp"
using namespace std;


class Particle {
    public: 
    Particle();
    Particle(CelPython* game, sf::Color color, int colorvel, int x, int y, int xvel, int yvel, int s, int svel);
    void update();
    void draw();

    CelPython* game;
    sf::Color color;
    int colorvel;
    int x;
    int y;
    int xvel;
    int yvel;
    int s;
    int svel;
};