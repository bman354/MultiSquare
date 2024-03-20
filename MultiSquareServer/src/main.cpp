#include <iostream>
#include <enet/enet.h>
#include <networking.h>
#include <vector>
#include <string>
#include <sstream>
#include <random>
/*
Packet Layouts

new client connects and sends its init data
NEW_CONNECTION: header;playerDesiredName

send back the server's player Id and starting positions
NEW_CONNECTION_ACKNOWLEDGE: header;id;pos.x;pos.y

player sends where they are, header ID also used to send back to clients to inform of updated pos on that player
POSITION_UPDATE: header;clientId;x;y;xvel;yvel



*/
std::vector<std::string> breakPacket(std::string packetData);
int newConnectionPacketHandler(ENetPeer* sender);

struct Client {
	ENetPeer* peer = {};
	int id;
	float x;
	float y;
	float xVel;
	float yVel;

	int lives;
	float health;
	float maxSpeed;
	float damage;
	float acceleration;
	float bulletSpeed;
};
int currentId = 0;
std::vector<Client> clients;


int main()
{
	
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

	

	std::cout << "Server Started, running on port " << address.port << "\n";
	//server main loop
	while (true) {


		ENetEvent event = {};

		while (enet_host_service(server, &event, 10) > 0)
		{
			std::cout << " event found of type: " << event.type << "\n";
			switch (event.type) {

				case ENET_EVENT_TYPE_CONNECT:
				{
					std::cout << "New client connected from address and port " << event.peer->address.host << " Port: " << event.peer->address.port << "\n";
					Client newClient;
					newClient.peer = event.peer;


					clients.push_back(newClient);
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{

					std::string rawPacket(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
					std::vector<std::string> packetData = breakPacket(rawPacket);
					ENetPeer* sender = event.peer;

					int packetHeader = std::stoi(packetData[0]);
					
					switch (packetHeader){

						case NEW_CONNECTION: {
							//new player data sent after ENET_EVENT_TYPE_CONNECT
							newConnectionPacketHandler(sender);
							break;
						}
						case POSITION_UPDATE: {
							//player sent an update on where they are


							break;
						}
					}
					enet_packet_destroy(event.packet);
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


		}
	}

	enet_host_destroy(server);
	return 0;
}


int sendMessage(const char* data, size_t size, ENetPeer* peer) {
	ENetPacket* packet = enet_packet_create(data, size, 0);
	return enet_peer_send(peer, 0, packet);
}

std::vector<std::string> breakPacket(std::string packetData) {
	// Simple tokenization example to parse packet content
	std::istringstream packetStream(packetData);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(packetStream, segment, ';'))
	{
		seglist.push_back(segment);
	}
	return seglist;
}

int newConnectionPacketHandler(ENetPeer* sender) {
	float initXPos = 100.0f;
	float initYPos = 100.0f;

	std::string newConnectionMessage = std::to_string(NEW_CONNECTION_ACKNOWLEDGE) + ";" + std::to_string(currentId)+ ";" + std::to_string(initXPos) + ";" + std::to_string(initYPos);
	std::cout << "Sending acknowledgement packet: " << newConnectionMessage << "\n";
	return sendMessage(newConnectionMessage.c_str(), newConnectionMessage.size() + 1, sender);
}
