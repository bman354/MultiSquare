#include "../include/Player.h"

// Constructor definition, -1 id means undefined on server side, server will send new ID
Player::Player() {
    pos = { 100.0f, 100.0f };
    velocity = { 0.0f, 0.0f };
    
    boostPower = 500.0f;
    boostVelocity = { 0.0f, 0.0f };
    boostTimer = 100;
    maxBoostTimer = boostTimer;
    maxBoostSpeed = 400.0f;

    id = -1; 
    name = "DEFAULT"; 
    health = 100.0f; 
    damage = 10.0f; 
    lives = 100;

    maxSpeed = 200.0f; 
    acceleration = 5.0f;
    fireRate = 100; 
    fireRateTimer = fireRate; 

    bulletSpeed = 500.0f; 

    playerSize = { 16.0f, 16.0f };
    bulletSize = { 8.0f, 8.0f };

}

void Player::update(float deltaTime)
{
    float friction = 0.99f;
    float boostFriction = 0.95f;

    velocity.x += boostVelocity.x;
    velocity.y += boostVelocity.y;

    boostVelocity *= boostFriction;

    if (glm::length(boostVelocity) > maxBoostSpeed) {
        boostVelocity = glm::normalize(boostVelocity) * maxBoostSpeed;
    }

    velocity *= friction;

    if (glm::length(velocity) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
    glm::vec2 totalVelocity = velocity + boostVelocity;

    pos += totalVelocity * deltaTime;
    
}

void Player::boost(glm::vec2 direction) {
    
    float angle = atan2(direction.y, direction.x);

    float boostXStrength = boostPower * cos(angle);
    float boostYStrength = boostPower * sin(angle);


    this->boostVelocity.x += boostXStrength;
    this->boostVelocity.y += boostYStrength;

}

ServerPlayer::ServerPlayer() {

    pos = { 100.0f, 100.0f };
    velocity = { 0.0f, 0.0f };

    boostPower = 500.0f;
    boostVelocity = { 0.0f, 0.0f };
    maxBoostSpeed = 400.0f;

    id = -1;
    name = "DEFAULT";
    health = 100.0f;
    damage = 10.0f;
    lives = 100;

    maxSpeed = 200.0f;
    acceleration = 5.0f;
    fireRate = 100;

    playerSize = { 16.0f, 16.0f };
    ENetPeer* peer = {};
}

ServerPlayer::ServerPlayer(Player playerData) {

    pos = playerData.pos;
    velocity = playerData.velocity;

    boostPower = playerData.boostPower;
    boostVelocity = playerData.boostVelocity;
    maxBoostTimer = playerData.maxBoostTimer;
    maxBoostSpeed = playerData.maxBoostSpeed;

    id = playerData.id;
    name = playerData.name;
    health = playerData.health;
    damage = playerData.damage;
    lives = playerData.lives;

    maxSpeed = playerData.maxSpeed;
    acceleration = playerData.acceleration;
    fireRate = playerData.fireRate;

    playerSize = playerData.playerSize;
    ENetPeer* peer = {};
}