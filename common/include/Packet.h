#pragma once
#include <enet/enet.h>
#include "Player.h"
#include "Bullet.h"


enum PacketHeader
{
	HANDSHAKE = 1000,
	HANDSHAKE_CONFIRM = 1010,
	PLAYER_UPDATE = 1001,
	NEW_CONNECTION_ACKNOWLEDGE = 1002,
	NEW_OUTSIDE_PLAYER_CONNECTED = 1003,
	SYNC_UPDATE = 1004
};

/*
To send data: create a new Packet
to give it data, create one of the data structs, then pass that into sendPacket as a (char *)&data
*/


//generic packet
struct Packet {
	int header = -1;
	char* getData() {
		return (char*)((&header) + 1);
	}
};



//data, just send the bits, BITCH
struct HandshakePacket {
	Player player;
};

struct HandshakeConfirmation {
	int id;
};



void sendPacket(ENetPeer* to, Packet p, const char* data, size_t size, bool reliable, int channel);
char* parsePacket(ENetEvent& event, Packet& p, size_t& dataSize);
