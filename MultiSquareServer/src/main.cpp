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

packet sent to clients to give them data on a new client that has connected
NEW_OUTSIDE_PLAYER_CONNECTED:  header;id;x;y;xVel;yVel;lives;health;maxSpeed;damage;acceleration;bulletSpeed


packet being sent to update everything about a player every 500 ticks to ensure everyone is in sync
SYNC_UPDATE: header;id;x;y;xVel;yVel;lives;health;maxSpeed;damage;acceleration;bulletSpeed


*/


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



std::vector<std::string> breakPacket(std::string packetData);
std::string clientPacketData(Client client);
int newConnectionPacketHandler(ENetPeer* sender);
void handlePacket(ENetPeer* sender, const std::string& packetData);
void syncPositions();
void positionUpdated(ENetPeer* peer, std::vector<std::string> *packetData);


int currentId = 0;
std::vector<Client> clients;

int syncPositionTimer = 500;





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


void handlePacket(ENetPeer* sender, const std::string& packetData) {
	void handlePacket(ENetPeer * sender, const std::string & packetData) {
		std::vector<std::string> packetParts = splitPacket(packetData);
		if (packetParts.empty())
			return;

		int packetHeader = std::stoi(packetParts[0]);
		switch (packetHeader) {
		case POSITION_UPDATE:
			// Handle position update packet
			break;
		default:
			// Handle other packet types as needed
			break;
		}
	}
}


int newConnectionPacketHandler(ENetPeer* sender) {
	float initXPos = 100.0f;
	float initYPos = 100.0f;

	std::string newConnectionMessage = std::to_string(NEW_CONNECTION_ACKNOWLEDGE) + ";" + std::to_string(currentId)+ ";" + std::to_string(initXPos) + ";" + std::to_string(initYPos);
	std::cout << "Sending acknowledgement packet: " << newConnectionMessage << "Player ID: " << currentId <<"\n";


	//send NEW_OUTSIDE_PLAYER_CONNECTED packet with relevent data
	//NEW_OUTSIDE_PLAYER_CONNECTED:  header; id; x; y; xVel; yVel; lives; health; maxSpeed; damage; acceleration; bulletSpeed
	for (const Client& existingClient : clients) {
		if (existingClient.peer != sender) { 
			
		}
	}

	

	return sendMessage(newConnectionMessage.c_str(), newConnectionMessage.size() + 1, sender);
}
//builds a string with the clients stats for various types of packets
std::string clientPacketData(Client client) {

	return	  std::to_string(client.id) +
		";" + std::to_string(client.x) +
		";" + std::to_string(client.y) +
		";" + std::to_string(client.xVel) +
		";" + std::to_string(client.yVel) +
		";" + std::to_string(client.lives) +
		";" + std::to_string(client.health) +
		";" + std::to_string(client.maxSpeed) +
		";" + std::to_string(client.damage) +
		";" + std::to_string(client.acceleration) +
		";" + std::to_string(client.bulletSpeed);
}

//send syncingClient's stats to each reciever client to keep everything in lockstep
void syncPositions() {

	if(!clients.empty()){

		for (const Client& syncingClient : clients) {

			for (const Client& recieverClient : clients) {
				
				std::string payload = std::to_string(SYNC_UPDATE) + ";" + clientPacketData(syncingClient);
				std::cout << "sync payload: " << payload << "\n";
				sendMessage(payload.c_str(), payload.length() + 1, recieverClient.peer);

				
			}
		}
	}
}


//player sent a packet with update data, send everyone where they are except the sending client
void positionUpdated(ENetPeer* peer ,std::vector<std::string> *packetData) {
	for (const Client &recievingClient: clients) {
		
		//HACK find better comparison, this can be a performance hog
		if (!(recievingClient.peer == peer)) {
			//POSITION_UPDATE: header; clientId; x; y; xvel; yvel
			std::string positionPayload = std::to_string(POSITION_UPDATE) + 
				";" + packetData->at(1) +
				";" + packetData->at(2) + 
				";" + packetData->at(3) + 
				";" + packetData->at(4) + 
				";" + packetData->at(5);
			
			sendMessage(positionPayload.c_str(), positionPayload.size() + 1, recievingClient.peer);
		}
	}
}

