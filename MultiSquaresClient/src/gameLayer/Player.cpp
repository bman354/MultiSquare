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

//create a player obj from a packet recieved from the server
Player::Player(std::string packetData) {
    std::istringstream ss(packetData);
    std::string token;
    std::vector<std::string> tokens;

    // tokenize packetData using ';' as delimiter
    while (std::getline(ss, token, ';')) {
        tokens.push_back(token);
    }

    id = std::stoi(tokens[1]);
    name = tokens[2];
    pos = glm::vec2(std::stof(tokens[3]), std::stof(tokens[4]));
    velocity = glm::vec2(std::stof(tokens[5]), std::stof(tokens[6]));
    lives = std::stoi(tokens[7]);
    health = std::stof(tokens[8]);
    maxSpeed = std::stof(tokens[9]);
    damage = std::stof(tokens[10]);
    acceleration = std::stof(tokens[11]);
    bulletSpeed = std::stof(tokens[12]);
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
//header;id;name;posx;posy;velx;vely;lives;health;maxSpeed;damage;bulletSpeed
//is there a better way to append the header using the ENUM in the gamelayer?
std::string Player::toNetworkDataPacket(int HEADERID) {
    return std::to_string(HEADERID) + ";"
        + std::to_string(id) + ";"
        + name + ";"
        + std::to_string(pos.x) + ";"
        + std::to_string(pos.y) + ";"
        + std::to_string(velocity.x) + ";"
        + std::to_string(velocity.y) + ";"
        + std::to_string(lives) + ";"
        + std::to_string(health) + ";"
        + std::to_string(maxSpeed) + ";"
        + std::to_string(damage) + ";"
        + std::to_string(playerSize.x) + ";"
        + std::to_string(playerSize.y);
}

void Player::updateStats(Player newStats) {
    id = newStats.id;
    pos = newStats.pos;
    velocity = newStats.velocity;
    name = newStats.name;
    lives = newStats.lives;
    health = newStats.health;
    maxSpeed = newStats.maxSpeed;
    damage = newStats.damage;
    acceleration = newStats.acceleration;
    playerSize = newStats.playerSize;
    fireRate = newStats.fireRate;
    fireRateTimer = newStats.fireRateTimer;
    bulletSpeed = newStats.bulletSpeed;
    bulletSize = newStats.bulletSize;
}

