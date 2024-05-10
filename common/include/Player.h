#pragma once
#include <glm/glm.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <enet/enet.h>

class Player {
public:
	Player();
	void update(float deltaTime);
	void updateStats(Player newStats);
	void boost(glm::vec2 direction);
	int id;

	glm::vec2 pos;
	glm::vec2 velocity;
	
	float boostPower;
	glm::vec2 boostVelocity;
	short boostTimer;
	short maxBoostTimer;
	float maxBoostSpeed;

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

struct ServerPlayer {
public:
	ServerPlayer();
	ServerPlayer(Player playerData);
	std::string toNetworkDataPacket(int HEADERID);
	int id;

	glm::vec2 pos;
	glm::vec2 velocity;
	float acceleration;

	float boostPower;
	glm::vec2 boostVelocity;
	short maxBoostTimer;
	float maxBoostSpeed;

	std::string name;

	int lives;
	float health;
	float maxSpeed;
	float damage;
	glm::vec2 playerSize;
	int fireRate;

	ENetPeer* peer;
};