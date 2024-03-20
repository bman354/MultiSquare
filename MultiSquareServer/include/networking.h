#pragma once
int sendMessage(const char *data, size_t s, ENetPeer *to);


constexpr size_t CHANNELS = 2;
constexpr size_t MAX_CLIENTS = 32;
enum PacketHeader
{
	NEW_CONNECTION = 0,
	POSITION_UPDATE = 1,
	NEW_CONNECTION_ACKNOWLEDGE = 2,
	NEW_OUTSIDE_PLAYER_CONNECTED = 3
};