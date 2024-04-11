#pragma once

enum PacketHeader
{
	NEW_CONNECTION = 10,
	PLAYER_UPDATE = 20,
	NEW_CONNECTION_ACKNOWLEDGE = 30,
	NEW_OUTSIDE_PLAYER_CONNECTED = 40,
	SYNC_UPDATE = 50
};


struct Packet {


	uint32_t header = -1;
	char *getData() {
		return (char*)(&header + 1);
	}



};

struct NewConnectionPacket {

};

struct NewConnectionAcknowledgePacket {

};

struct NewPlayerConnectionPacket {

};

struct PlayerUpdatePacket {

};

struct SyncPacket {

};



