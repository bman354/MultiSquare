#pragma once
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>
#include <string>




class Player {
public:
	Player();
	void update(float deltaTime);
	std::string newConnectionDataPacket();
	int id;

	glm::vec2 pos;
	glm::vec2 velocity;

	std::string name;

	int lives;
	float health;
	float maxSpeed;
	float damage;
	float acceleration;

	int fireRate;
	int fireRateTimer;

	float bulletSpeed;

	glm::vec2 bulletSize;
	glm::vec2 playerSize;
};