#include "../include/Bullet.h"

// Constructor definitions
Bullet::Bullet() {

}

Bullet::Bullet(Player playerStats):
    width(playerStats.bulletSize.x),
    height(playerStats.bulletSize.y),
    speed(playerStats.bulletSpeed),
    damage(playerStats.damage),
    pos(playerStats.pos){

    glm::vec2 bulletVelocity = glm::normalize(this->velocity);
    bulletVelocity *= playerStats.bulletSpeed;
    bulletVelocity.x *= (playerStats.velocity.x / 2.0f);
    bulletVelocity.y *= (playerStats.velocity.y / 2.0f);

    velocity = bulletVelocity;
}

void Bullet::update(){
    this->pos += this->velocity;
}

//HEADER;x;y;xVel;yVel;width;height;speed;damage
std::string Bullet::toNetworkPacket(std::string packetHeader) {

    return packetHeader + ";"
        + std::to_string(pos.x)
        + std::to_string(pos.y)
        + std::to_string(velocity.x)
        + std::to_string(velocity.y)
        + std::to_string(width)
        + std::to_string(height)
        + std::to_string(speed)
        + std::to_string(damage);

}



Bullet Bullet::packetToBullet(std::string packetData) {
	Bullet returnBullet;
	
	std::istringstream ss(packetData);
	std::string token;
	std::vector<std::string> tokens;

	// tokenize packetData using ';' as delimiter
	while (std::getline(ss, token, ';')) {
		tokens.push_back(token);
	}

	returnBullet.pos.x = std::stof(tokens[1]);
    returnBullet.pos.y = std::stof(tokens[2]);

    returnBullet.velocity.x = std::stof(tokens[3]);
    returnBullet.velocity.y = std::stof(tokens[4]);

    returnBullet.width = std::stof(tokens[5]);
    returnBullet.height = std::stof(tokens[6]);
    returnBullet.speed = std::stof(tokens[7]);
    returnBullet.damage = std::stof(tokens[8]);

	return returnBullet;
}