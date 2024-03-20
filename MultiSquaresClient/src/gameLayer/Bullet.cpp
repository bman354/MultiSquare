#include "Bullet.h"

// Constructor definition
Bullet::Bullet(Player playerStats):
    width(playerStats.bulletSize.x),
    height(playerStats.bulletSize.y),
    speed(playerStats.bulletSpeed),
    damage(playerStats.damage){}

void Bullet::update(){
    this->pos += this->velocity;
}