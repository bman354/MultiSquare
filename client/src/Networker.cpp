#include "Networker.h"

int Networker::initNetworker(char ipAddress[30], char port[20], Player& player, std::vector<Player>& extPlayers) {
	
	if (enet_initialize() != 0) {
		std::cout << "Enet failed to initialize\n";
		return -1;
	}

	this->client = enet_host_create(NULL, 1, 2, 0, 0);

	if (this->client == NULL) {
		std::cout << "client failed to be created\n";
		return -1;
	}

	enet_address_set_host(&address, ipAddress);
	address.port = atoi(port);

	server = enet_host_connect(this->client, &address, 2, 0);

	if (server == nullptr) {
		std::cout << "Failed to initialize connection to server\n";
		return -1;
	}
	else {
		event = {};
		//Enet Handshake
		if (enet_host_service(this->client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
			std::cout << "Enet handshake successful\n";
		}

		//Gamelayer Handshake
		Packet packet;
		packet.header = HANDSHAKE;

		HandshakePacket packetData;
		packetData.player = player;

		std::cout << "sending handshake\n";
		sendPacket(server, packet, (char*)&packetData, sizeof(packetData), true, 1);

	}
}


void Networker::doEnetEventService(Player& player, std::vector<Player>& extPlayers, std::vector<Bullet>& bullets) {
	while (enet_host_service(this->client, &event, 0) > 0) {
		switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT: {
				//HACK may not be useful here?
				std::cout << "Connected to server!" << std::endl;
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT: {
				std::cout << "Disconnected from server!" << std::endl;
				//TODO actually disconnect game
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE: {

				Packet packet;
				size_t dataSize = 0;

				auto data = parsePacket(event, packet, dataSize);


				switch (packet.header) {
					case NEW_PLAYER_CONNECTED: {
						NewPlayerConnectedPacket newConnectedPacket = *(NewPlayerConnectedPacket*)data;
						extPlayers.push_back(newConnectedPacket.connectingPlayer);
						break;
					} case PLAYER_POS_UPDATE: {
						PosUpdatePacket posPacket = *(PosUpdatePacket*)data;
						handlePlayerPosUpdate(posPacket, extPlayers);
						break;
					} case HANDSHAKE_PLAYERDATA: {
						std::cout << "recieved handshake data\n";
						HandshakeDataPacket handshakePacket = *(HandshakeDataPacket*)data;
						for (Player player : handshakePacket.existingPlayers) {
							extPlayers.push_back(player);
						}
						break;
					}
				}
				break;
			}
		}
	}
}

void Networker::closeNetworker() {
	// Send a disconnect message to the server
	if (this->client != nullptr && server != nullptr) {
		enet_peer_disconnect(server, 0);
		// Allow up to 3 seconds for the disconnect to succeed
		enet_host_flush(this->client);
	}

	// Cleanup resources
	enet_host_destroy(this->client);
	enet_deinitialize();
}

void Networker::sendPlayerPosUpdate(Player& player) {

	Packet packet;
	packet.header = PLAYER_POS_UPDATE;

	PosUpdatePacket packetData;
	packetData.id = player.id;
	packetData.x = player.pos.x;
	packetData.y = player.pos.y;
	packetData.xVel = player.velocity.x;
	packetData.xVel = player.velocity.y;

	sendPacket(server, packet, (char*)&packetData, sizeof(packetData), true, 1);
}

void Networker::handlePlayerPosUpdate(PosUpdatePacket& posPacket, std::vector<Player>& extPlayers) {
	for (Player& player : extPlayers) {
		if (player.id == posPacket.id) {
			player.pos.x = posPacket.x;
			player.pos.y = posPacket.y;
			player.velocity.x = posPacket.xVel;
			player.velocity.y = posPacket.yVel;
		}
	}
}