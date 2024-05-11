#pragma once

#include <enet/enet.h>
#include "Packet.h"
#include "Player.h"
#include "Bullet.h"
#include <iostream>

struct Networker {
	ENetHost* client = nullptr;
	ENetPeer* server = nullptr;
	ENetAddress address = {};
	ENetEvent event = {};

	unsigned long long serverTime = 0;

	int initNetworker(char ipAddress[30], char port[20], Player& player);
	void doEnetEventService(Player& player, std::vector<Player>& extPlayers, std::vector<Bullet>& bullets);
	void closeNetworker();
	void sendPlayerPosUpdate(Player& player);
	void handlePlayerPosUpdate(PosUpdatePacket& posPacket, std::vector<Player>& extPlayers);
};
