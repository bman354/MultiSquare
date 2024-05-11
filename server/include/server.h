#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Packet.h"

constexpr size_t CHANNELS = 2;
constexpr size_t MAX_CLIENTS = 32;

void doServerHandshake(HandshakePacket& handshakePacket, ENetEvent* event);
void playerPosUpdate(PosUpdatePacket& posPacket, ENetEvent* event);