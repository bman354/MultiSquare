#pragma once
#include <enet/enet.h>
#include "Player.h"
#include "Bullet.h"


enum PacketHeader
{
	HANDSHAKE = 1000,
	HANDSHAKE_CONFIRM = 1010,

	NEW_PLAYER_CONNECTED = 1001,
	HANDSHAKE_PLAYERDATA = 1011,

	PLAYER_POS_UPDATE = 1002
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

//sent to server with new player's data
struct HandshakePacket {
	Player player;
};

//sent from server with new id
struct HandshakeConfirmationPacket {
	int id;
};

//sent from server to alert to new player connected
struct NewPlayerConnectedPacket {
	Player connectingPlayer;
};

struct GenericPlayerPacket {
	Player playerData;
};

struct PosUpdatePacket {
	int id;
	float x;
	float y;
	float xVel;
	float yVel;
};

void sendPacket(ENetPeer* to, Packet p, const char* data, size_t size, bool reliable, int channel);
char* parsePacket(ENetEvent& event, Packet& p, size_t& dataSize);
