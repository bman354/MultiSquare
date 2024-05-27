#include <iostream>
#include <enet/enet.h>
#include <server.h>
#include <vector>
#include <string>


unsigned long long serverTime = 0;
int currentId = 0;
std::vector<Client> clients = {};

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
	for (Client& player : clients) {
		sendPacket(player.peer, p, data, size, reliable, channel);
	}
}

void broadcastAll(ENetPeer* ignoredPeer, Packet p, const char* data, size_t size, bool reliable, int channel) {
	for (Client& client : clients) {
		if (client.peer != ignoredPeer) {
			sendPacket(client.peer, p, data, size, reliable, channel);
		}
	}
}

void broadcastAll(int ignoredId, Packet p, const char* data, size_t size, bool reliable, int channel) {
	for (Client& client : clients) {
		if (client.player.id != ignoredId) {
			sendPacket(client.peer, p, data, size, reliable, channel);
		}
	}
}

//send back newly connected client their id, increment id and send new player's information to all other clients
void doServerHandshake(HandshakePacket& handshakePacket, ENetEvent* event) {
	Packet p;
	p.header = HANDSHAKE_CONFIRM;

	ConfirmHandshakePacket data;
	data.id = currentId;
	data.serverTime = serverTime;

	Client newClient;
	newClient.player = handshakePacket.player;
	newClient.player.id = data.id;
	newClient.peer = event->peer;
	
	size_t i = 0;
	for (Client client : clients) {
		data.existingPlayers[i] = client.player;
		i++;
	}
	
	//send handshake confirmation
	sendPacket(newClient.peer, p, (char*)&data, sizeof(data), true, 1);

	Packet newPlayerPacket;
	newPlayerPacket.header = NEW_PLAYER_CONNECTED;
	
	PlayerPacket newPlayerData;
	newPlayerData.player = newClient.player;

	broadcastAll(newClient.player.id, newPlayerPacket, (char*)&newPlayerData, sizeof(newPlayerData), true, 1);

	clients.push_back(newClient);
	currentId++;
}


void playerPosUpdate(PosUpdatePacket& posPacket, ENetEvent* event) {
    Packet p;
    p.header = PLAYER_POS_UPDATE;

    for (Client& client : clients) {
        if (client.peer == event->peer) {
            // Update position for the player associated with the received position update packet
            client.player.pos.x = posPacket.x;
            client.player.pos.y = posPacket.y;
            client.player.velocity.x = posPacket.xVel;
            client.player.velocity.y = posPacket.yVel;

            // Broadcast the updated position to all other clients
            broadcastAll(client.player.id, p, (char*)&posPacket, sizeof(posPacket), true, 1);
        }
    }
}
