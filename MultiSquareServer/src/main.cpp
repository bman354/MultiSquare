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
NEW_CONNECTION: header;id;name;x;y;xVel;yVel;lives;health;maxSpeed;damage;acceleration;bulletSpeed

send back the server's player Id and starting positions
NEW_CONNECTION_ACKNOWLEDGE: header;id;pos.x;pos.y

player sends where they are, header ID also used to send back to clients to inform of updated pos on that player
PLAYER_UPDATE: header;id;name;x;y;xVel;yVel;lives;health;maxSpeed;damage;xSize;ySize

packet sent to clients to give them data on a new client that has connected
NEW_OUTSIDE_PLAYER_CONNECTED:  header;id;name;x;y;xVel;yVel;lives;health;maxSpeed;damage;xSize;ySize

packet being sent to update everything about a player every 500 ticks to ensure everyone is in sync
SYNC_UPDATE: header;id;name;x;y;xVel;yVel;lives;health;maxSpeed;damage;xSize;ySize
*/


struct Client {
	ENetPeer* peer = {};
	int id;
	std::string name;
	float x;
	float y;
	float xVel;
	float yVel;
	float xSize;
	float ySize;

	int lives;
	float health;
	float maxSpeed;
	float damage;
	float acceleration;
	float bulletSpeed;
};


int newConnectionPacketHandler(ENetPeer* sender);
void handlePacket(ENetPeer* sender, const std::string& packetData);
bool syncPositions();
bool sendClientUpdate(Client client);
std::string clientToPacket(Client client);

int currentId = 0;
std::vector<Client> clients;

int syncPositionTimer = 5000;





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
			switch (event.type) {

				case ENET_EVENT_TYPE_CONNECT:
				{
					std::cout << "New client connected from address and port " << event.peer->address.host << " Port: " << event.peer->address.port << "\n";
					Client newClient;
					newClient.peer = event.peer;
					newClient.id = currentId;
					currentId++;
					clients.push_back(newClient);
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					handlePacket(event.peer, std::string(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength));
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

		//syncing and common routines
		if(clients.size() > 1) {
			if (syncPositionTimer > 0) {
				syncPositionTimer--;
			}
			else if (syncPositionTimer <= 0) {

				syncPositions();
				syncPositionTimer = 500;

			}
		}
		//end of main server loop, clean up after here
	}

	enet_host_destroy(server);
	return 0;
}

int sendMessage(const char* data, size_t size, ENetPeer* peer) {

	if (std::strlen(data) > 500) {
		std::cout << "WARNING, packet was sent with size > 500, data loss may occur\n";
	}

	ENetPacket* packet = enet_packet_create(data, size, 0);
	return enet_peer_send(peer, 0, packet);
}


void handlePacket(ENetPeer* sender, const std::string& packetData) {
	
		int packetHeader = getPacketHeader(packetData);
		
		switch (packetHeader) {
			case PLAYER_UPDATE: {
				Client updateClient = packetToClient(packetData);
				sendClientUpdate(updateClient);
				break;
			}
		default:
			// Handle other packet types as needed
			break;
		}
	
}


int newConnectionPacketHandler(ENetPeer* sender) {
	
}



//send syncingClient's stats to each reciever client to keep everything in lockstep
bool syncPositions() {
	bool sentSuccessfully = true;
	if (!clients.empty()) {

		for (const Client& syncingClient : clients) {

			for (const Client& recieverClient : clients) {

				std::string payload = std::to_string(SYNC_UPDATE) + ";" + clientToPacket(syncingClient);
				std::cout << "sync payload: " << payload << "\n";
				if (sendMessage(payload.c_str(), payload.length() + 1, recieverClient.peer) < 0) {
					sentSuccessfully = false;
				}
			}
		}
	}
	return sentSuccessfully;
}

//player sent a packet with update data, send everyone but sender fresh data
bool sendClientUpdate(Client senderClient) {
	bool sentSuccessfully = true;
	for (const Client& recievingClient : clients) {
		if (recievingClient.id != senderClient.id) {
			std::string payload = std::to_string(PLAYER_UPDATE) + ";";
			payload += clientToPacket(senderClient);

			if ((sendMessage(payload.c_str(), strlen(payload.c_str()) + 1, recievingClient.peer)) < 0) {
				sentSuccessfully = false;
			}
		}
	}
}


//builds a string with the clients stats for various types of packets
std::string clientToPacket(Client client) {

	return std::to_string(client.id) + ";"
		+ client.name + ";"
		+ std::to_string(client.x) + ";"
		+ std::to_string(client.y) + ";"
		+ std::to_string(client.xVel) + ";"
		+ std::to_string(client.yVel) + ";"
		+ std::to_string(client.lives) + ";"
		+ std::to_string(client.health) + ";"
		+ std::to_string(client.maxSpeed) + ";"
		+ std::to_string(client.damage) + ";"
		+ std::to_string(client.xSize) + ";"
		+ std::to_string(client.ySize);
}

Client packetToClient(std::string packetData) {
	Client returnClient;

	std::istringstream ss(packetData);
	std::string token;
	std::vector<std::string> tokens;

	// tokenize packetData using ';' as delimiter
	while (std::getline(ss, token, ';')) {
		tokens.push_back(token);
	}

	returnClient.id = std::stoi(tokens[1]);
	returnClient.name = tokens[2];
	returnClient.x = std::stof(tokens[3]);;
	returnClient.y = std::stof(tokens[4]);
	returnClient.xVel = std::stof(tokens[5]);
	returnClient.yVel = std::stof(tokens[6]);
	returnClient.lives = std::stoi(tokens[7]);
	returnClient.health = std::stof(tokens[8]);
	returnClient.maxSpeed = std::stof(tokens[9]);
	returnClient.damage = std::stof(tokens[10]);
	returnClient.acceleration = std::stof(tokens[11]);
	returnClient.bulletSpeed = std::stof(tokens[12]);

	return returnClient;
}

int getPacketHeader(const std::string& packetData) {
	size_t delimiterPos = packetData.find(';');

	std::string headerString = packetData.substr(0, delimiterPos);

	return std::stoi(headerString);
}