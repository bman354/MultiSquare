#include "Player.h"

// Constructor definition
Player::Player() {
    pos = { 100.0f, 100.0f };
    velocity = { 0.0f, 0.0f };
    id = 0; 
    name = "DEFAULT"; 
    health = 100.0f; 
    damage = 10.0f; 
    lives = 100;

    maxSpeed = 100.0f; 
    acceleration = 25.0f;
    fireRate = 100; 
    fireRateTimer = fireRate; 

    bulletSpeed = 10.0f; 

    playerSize = { 16.0f, 16.0f };
    bulletSize = { 16.0f, 16.0f };

}

void Player::update(float deltaTime)
{
    float friction = 0.96f;

    velocity *= friction;

    if (glm::length(velocity) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
    if (velocity.x < 0.05f && velocity.x > -0.05f) {
        velocity.x = 0;
    }
    if (velocity.y < 0.05f && velocity.y > -0.05f) {
        velocity.y = 0.0f;
    }


    pos += velocity * deltaTime;
    
}

std::string Player::newConnectionDataPacket() {
    return name + ";"
        + std::to_string(lives) + ";"
        + std::to_string(health) + ";"
        + std::to_string(maxSpeed) + ";"
        + std::to_string(damage) + ";"
        + std::to_string(bulletSpeed);
}