#pragma once
#include <gl2d/gl2d.h>
#include "Player.h"
#include <glm/glm.hpp>
#include <sstream>


class Bullet {
public:
    // Constructor
    Bullet(Player playerStats);
    Bullet();
    std::string toNetworkPacket(std::string packetHeader);
    Bullet packetToBullet(std::string packetData);
    void update();
    
    glm::vec2 velocity = {0,0};
    glm::vec2 pos = { 0,0 };


    float width;
    float height;
    //rotation
    float speed;
    float damage;
};
