#include <iostream>
#include <enet/enet.h>
#include <server.h>
#include <vector>
#include <string>


unsigned long long serverTime = 0;
int currentId = 0;
std::vector<ServerPlayer> clients;

int main()
{
#pragma region init
	if (enet_initialize() != 0) {
		std::cerr << "Failed to initialize ENet\n";
		return -1;
	}

	ENetAddress address = {};
	ENetHost* server = nullptr;
	address.host = ENET_HOST_ANY; //we accept cleints from any ip address since we are the server
	address.port = 42040;


	server = enet_host_create(&address /* the address to bind the server host to */,
		MAX_CLIENTS			/* allow up to 32 clients and/or outgoing connections */,
		CHANNELS	/* allow up to 2 channels to be used, 0 and 1 */,
		0			/* assume any amount of incoming bandwidth */,
		0			/* assume any amount of outgoing bandwidth */);

	if (server == NULL)
	{
		std::cout << "server is null\n";
		return -1;
	}

	
#pragma endregion

	std::cout << "Server Started, running on port " << address.port << "\n";
	//server main loop
	while (true) {
		ENetEvent event = {};
		int eventsProcessed = 0;

		while (enet_host_service(server, &event, 10) > 0) {
			if (eventsProcessed < 10) {
				switch (event.type) {

				case ENET_EVENT_TYPE_CONNECT: {
					std::cout << "New client connected from address and port " << event.peer->address.host << " Port: " << event.peer->address.port << "\n";
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE: {
					//packet holds the header, packetsize is how big the data is
					//data points to the start of the data
					Packet packet;
					size_t dataSize = 0;

					auto data = parsePacket(event, packet, dataSize);

					switch (packet.header) {
						case(HANDSHAKE): {
							HandshakePacket handshakePacket = *(HandshakePacket*)data;
							doServerHandshake(handshakePacket, &event);
							break;
						} case(PLAYER_POS_UPDATE) : {
							PosUpdatePacket posPacket = *(PosUpdatePacket*)data;
							playerPosUpdate(posPacket, &event);
							break;
						}
					}
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT: {
					for (auto it = clients.begin(); it != clients.end(); ++it) {
						if (it->peer == event.peer) {
							std::cout << "Client disconnected\n";
							clients.erase(it);
							break;
						}
					}
					break;
				}

				}
				//process a max of 10 events before moving on to avoid too many packets clogging server
				eventsProcessed++;
			} else {
				break;
			}
		}
		serverTime++;
		//end of main server loop, clean up after here
	}
	
	enet_host_destroy(server);
	return 0;
}

void broadcastAll(Packet p, const char* data, size_t size, bool reliable, int channel) {
	for (ServerPlayer& player : clients) {
		sendPacket(player.peer, p, data, size, reliable, channel);
	}
}
void broadcastAll(ENetPeer *ignoredPeer, Packet p, const char* data, size_t size, bool reliable, int channel) {
	for (ServerPlayer& player : clients) {
		if (player.peer != ignoredPeer) {
			sendPacket(player.peer, p, data, size, reliable, channel);
		}
	}
}
void broadcastAll(int ignoredId, Packet p, const char* data, size_t size, bool reliable, int channel) {
	for (ServerPlayer& player : clients) {
		if (player.id != ignoredId) {
			sendPacket(player.peer, p, data, size, reliable, channel);
		}
	}
}

//send back newly connected client their id, increment id and send new player's information to all other clients
void doServerHandshake(HandshakePacket& handshakePacket, ENetEvent* event) {

	ServerPlayer newPlayer(handshakePacket.player);

	newPlayer.id = currentId;
	currentId++;
	newPlayer.peer = event->peer;

	clients.push_back(newPlayer);

	Packet packet;
	packet.header = NEW_PLAYER_CONNECTED;

	NewPlayerConnectedPacket packetData;
	packetData.connectingPlayer = serverPlayerToPlayer(newPlayer);

	//send to all, ignore new player
	broadcastAll(newPlayer.peer, packet, (char*)&packetData, sizeof(packetData), true, 1);
	
	packet = {};
	packet.header = HANDSHAKE_CONFIRM;

	HandshakeConfirmationPacket handshakePacketData;
	handshakePacketData.id = newPlayer.id;

	sendPacket(newPlayer.peer, packet, (char*)&handshakePacketData, sizeof(handshakePacketData), true, 1);
	std::cout << "accepted handshake with Id: " << newPlayer.id << "\n";
}

void playerPosUpdate(PosUpdatePacket& posPacket, ENetEvent* event) {
	
	Packet p;
	p.header = PLAYER_POS_UPDATE;
	
	for (ServerPlayer player : clients) {
		if (posPacket.id == player.id) {
			player.pos.x = posPacket.x;
			player.pos.y = posPacket.y;
			player.velocity.x = posPacket.xVel;
			player.velocity.y = posPacket.yVel;
			break;
		}
		else {
			sendPacket(player.peer, p, (char*)&posPacket, sizeof(posPacket), true, 1);
		}
	}
}

