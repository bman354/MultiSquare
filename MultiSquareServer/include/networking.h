#pragma once
int sendMessage(const char *data, size_t s, ENetPeer *to);


constexpr size_t CHANNELS = 2;
constexpr size_t MAX_CLIENTS = 32;
enum PacketHeader
{
	NEW_CONNECTION = 10,
	POSITION_UPDATE = 20,
	NEW_CONNECTION_ACKNOWLEDGE = 30,
	NEW_OUTSIDE_PLAYER_CONNECTED = 40,
	SYNC_UPDATE = 50
};