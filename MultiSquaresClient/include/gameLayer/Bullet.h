#pragma once
#include <gl2d/gl2d.h>
#include <Player.h>
#include <glm/glm.hpp>


class Bullet {
public:
    // Constructor
    Bullet(Player playerStats);
    void update();


    glm::vec2 velocity = {0,0};
    glm::vec2 pos = { 0,0 };


    float width;
    float height;

    float speed;
    float damage;
};
