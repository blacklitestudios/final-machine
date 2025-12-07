#include "particle.hpp"

Particle::Particle(CelPython* game, sf::Color color, int colorvel, int x, int y, int xvel, int yvel, int s, int svel) {
    this->game = game;
    this->color = color;
    this->colorvel = colorvel;
    this->x = x;
    this->y = y;
    this->xvel = xvel;
    this->yvel = yvel;
    this->s = s;
    this->svel = svel;
}

