#pragma once
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>
#include <string>
#include <sstream>
#include <Bullet.h>



class Player {
public:
	Player();
	Player(std::string packetData);
	void update(float deltaTime);
	void updateStats(Player newStats);
	Bullet fireBullet();

	std::string toNetworkDataPacket(int HEADERID);
	int id;

	glm::vec2 pos;
	glm::vec2 velocity;

	std::string name;

	int lives;
	float health;
	float maxSpeed;
	float damage;
	float acceleration;
	glm::vec2 playerSize;

	int fireRate;
	int fireRateTimer;

	float bulletSpeed;

	glm::vec2 bulletSize;
};