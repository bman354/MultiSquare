#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
//#include "imgui.h"
#include <iostream>
#include <sstream>
#include <enet/enet.h>
//#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <renderer.h>
#include <cstdio>
#include <glui/glui.h>
#include <raudio.h>

#include "Bullet.h"
#include "Player.h"

//TODO Client state into int ENUM? mostly on serverSide
/*
enum GAME_STATE{

	MAIN_MENU



}
*/


enum PacketHeader
{
	NEW_CONNECTION = 10,
	PLAYER_UPDATE = 20,
	NEW_CONNECTION_ACKNOWLEDGE = 30,
	NEW_OUTSIDE_PLAYER_CONNECTED = 40,
	SYNC_UPDATE = 50
};

//TODO figure out a structure to hold all this data, this is too much just sitting around in an odd scope
//use one structure that is just players, "player" can just be an index or a key to the local player?
Player player;
std::vector<Bullet> bullets;
std::vector<Player> extPlayers;

gl2d::Renderer2D renderer;
glui::RendererUi rendererUi;

gl2d::Texture playerTexture;
gl2d::Texture bulletTexture;
gl2d::Texture menuTexture;

gl2d::Font font;

ENetHost* client = nullptr;
ENetPeer* server = nullptr;
ENetAddress address = {};
ENetEvent event = {};

char ipAddress[30];
char port[20];
char playerMenuName[30];

std::string newPlayerName = "";
std::string ipToConnectTo = "";
std::string portToConnectTo = "";

//timers
int sendPosTimer = 10;


//GAMEPLAY FLAGS~~~~~~
//TODO swap this to a single state ENUM/Variable so I can use a switch statement instead of if/else
bool IN_MAIN_MENU = true;
bool IN_GAME = false;
bool IS_CONNECTED = false;
int selected = 0;

bool initGame() {
	//creates the renderer and starts game
	gl2d::init();
	renderer.create();

	playerTexture.loadFromFile(RESOURCES_PATH "playerSkins/default.png");
	bulletTexture.loadFromFile(RESOURCES_PATH "bulletSkins/default.png");
	menuTexture.loadFromFile(RESOURCES_PATH "playerSkins/default.png");
	font.createFromFile(RESOURCES_PATH "Arial.ttf");

	return true;
}

bool gameLogic(float deltaTime) {

#pragma region init stuff
	//clear the screen and reset the renderer based on the window's current size
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion


#pragma region Main Menu


	if (IN_MAIN_MENU) {
		rendererUi.Begin(1);

		rendererUi.Text("MultiSquares", Colors_White);
		rendererUi.BeginMenu("Play", Colors_Blue, menuTexture);
		if (rendererUi.Button("Connect", Colors_Blue)) {


			strcpy(ipAddress, ipToConnectTo.c_str());
			strcpy(port, portToConnectTo.c_str());
			strcpy(playerMenuName, newPlayerName.c_str());

			IN_MAIN_MENU = false;

		}

		rendererUi.newColum(1243);
		if (rendererUi.Button("Name", Colors_Black)) {
			selected = 1;
		}
		if (rendererUi.Button("IP", Colors_Black)) {
			selected = 2;
		}
		if (rendererUi.Button("Port", Colors_Black)) {
			selected = 3;
		}

		rendererUi.newColum(5234);
		rendererUi.Text(newPlayerName + "##01", Colors_Black);
		rendererUi.Text(ipToConnectTo + "##02", Colors_Black);
		rendererUi.Text(portToConnectTo + "##03", Colors_Black);



		//key input handling
		std::string in = platform::getTypedInput();
		//TODO this doesnt work yet
		int ctrlPressed = platform::isButtonPressedOn(43);
		for (char c : in) {
			if (c == '\b') {
				switch (selected) {
				case 1: // Name
					if (!newPlayerName.empty()) {
						newPlayerName.pop_back();
					}
					break;
				case 2: // IP
					if (!ipToConnectTo.empty()) {
						ipToConnectTo.pop_back();
					}
					break;
				case 3: // Port
					if (!portToConnectTo.empty()) {
						portToConnectTo.pop_back();
					}
					break;
				}
			}
			else {
				if (in.size()) {
					switch (selected) {
					case 1:
						newPlayerName += in;
						break;
					case 2:
						ipToConnectTo += in;
						break;
					case 3:
						portToConnectTo += in;
						break;
					}

				}
			}
		}


		rendererUi.EndMenu();


		rendererUi.End();
		renderer.clearScreen(Colors_White);
		rendererUi.renderFrame(renderer, font, platform::getRelMousePosition(), platform::isLMousePressed(), platform::isLMouseHeld(), platform::isLMouseReleased(),
			platform::isButtonReleased(platform::Button::Escape), platform::getTypedInput(), deltaTime);
		renderer.flush();
	}
	else {

#pragma endregion

#pragma region connection init
		//set up connection
		if (!IS_CONNECTED) {
			client = enet_host_create(NULL, 1, 2, 0, 0);

			if (client == NULL) {
				std::cout << "client failed to be created\n";
				IN_MAIN_MENU = true;
				IS_CONNECTED = false;
			}

			enet_address_set_host(&address, ipAddress);
			address.port = atoi(port);

			server = enet_host_connect(client, &address, 2, 0);

			if (server == nullptr) {
				std::cout << "Failed to initialize connection\n";
				IN_MAIN_MENU = true;
				IS_CONNECTED = false;
			}
			else {
				player.name = playerMenuName;
				event = {};
				//wait for acknowledgement from enet
				if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
					std::cout << "Connection success!\n";
					IS_CONNECTED = true;

					
				}

				//tell server we are new, and need to tell everyone we've joined
				sendPacket(player.toNetworkDataPacket(NEW_CONNECTION));
				std::cout << "sent new connection packet\n";
				event = {};
				if (enet_host_service(client, &event, 5000) > 0) {
					std::string rawPacket(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
					int packetHeader = getPacketHeader(rawPacket);
					
					if (packetHeader == NEW_CONNECTION_ACKNOWLEDGE) {
						newConnectionAcknowledge(rawPacket);
						std::cout << "connection acknowledged and ip applied\n";
					}

				} else {
					std::cout << "connection failed to be acknowledged\n";
					return false;
				}
			}
		}
		else {
#pragma endregion


#pragma region movement input

			glm::vec2 inputAcceleration = { 0.0f, 0.0f };

			if (platform::isButtonHeld(platform::Button::W)) {
				inputAcceleration.y -= player.acceleration;
			}
			if (
				platform::isButtonHeld(platform::Button::S)) {
				inputAcceleration.y += player.acceleration;
			}
			if (
				platform::isButtonHeld(platform::Button::A)) {
				inputAcceleration.x -= player.acceleration;
			}
			if (
				platform::isButtonHeld(platform::Button::D)) {
				inputAcceleration.x += player.acceleration;
			}
			
			/*
			if (
				platform::isLMousePressed || platform::isLMouseHeld) {
				Bullet firedBullet;
				bullets.push_back(firedBullet.fireBullet(player));
			} 
			*/
			
			player.velocity += inputAcceleration;
			player.update(deltaTime);

			//update the server semi regularly with the player's data
			//HACK change this to only send packets when player info changes enough
			if (sendPosTimer >= 1) {
				sendPosTimer--;
			}
			else {
				sendPosUpdate();
				sendPosTimer = 1;
			}


#pragma endregion


#pragma region network event handler
			//HACK probably needs to either go to another thread, ENET may do this already
			while (enet_host_service(client, &event, 0) > 0) {

				std::string rawPacket(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
				int packetHeader = getPacketHeader(rawPacket);

				switch (event.type) {

				case ENET_EVENT_TYPE_CONNECT: {
					std::cout << "Connected to server!" << std::endl;
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT: {
					std::cout << "Disconnected from server!" << std::endl;
					IN_MAIN_MENU = true;
					IS_CONNECTED = false;
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE: {

					switch (packetHeader) {
						case PLAYER_UPDATE: {
							playerUpdate(rawPacket);
							break;
						}
						case NEW_OUTSIDE_PLAYER_CONNECTED: {
							newPlayerConnected(rawPacket);
						break;
						}
					}
					
					break;
				}
				}
			}
#pragma endregion			

#pragma region rendering
		
		for (Bullet bullet : bullets) {
			renderBullet(renderer, bullet, bulletTexture);
		}

		for (Player player : extPlayers) {
			renderPlayer(renderer, player, playerTexture);
			renderPlayerName(renderer, player, font);
		}
		
		renderPlayer(renderer, player, playerTexture);
		renderPlayerName(renderer, player, font);

		//tell GPU to process everything
		renderer.flush();
#pragma endregion
			}
			return true;
		}
	}


//This function might not be be called if the program is forced closed
void closeGame()
{
	// Send a disconnect message to the server
	if (client != nullptr && server != nullptr) {
		enet_peer_disconnect(server, 0);
		// Allow up to 3 seconds for the disconnect to succeed
		enet_host_flush(client);
	}

	// Cleanup resources
	enet_host_destroy(client);
	enet_deinitialize();

}

#pragma region packet handling
void sendPacket(std::string data) {
	ENetPacket* packet = enet_packet_create(data.c_str(), data.size() + 1, 0);
	enet_peer_send(server, 0, packet);
}
void sendPacketReliable(std::string data) {
	ENetPacket* packet = enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(server, 0, packet);
}

void sendPosUpdate() {
	sendPacket(player.toNetworkDataPacket(PLAYER_UPDATE));
}

//ext player sent some data we need to update
void playerUpdate(std::string newData) {
	bool playerUpdated = false;
	Player newPlayer = Player(newData);
	
	for (Player &player : extPlayers) {
		if (player.id == newPlayer.id) {
			playerUpdated = true;
			player.updateStats(newPlayer);
		}
	}
	if (playerUpdated == false) {
		newPlayerConnected(newData);
	}
}


void newPlayerConnected(std::string packetData) {
	std::cout << "new player connected\n";
	Player newPlayer = Player(packetData);
	std::cout << "new player id: " << newPlayer.id << "\n";
	if (extPlayers.size() > 0) {
		for (const Player& player : extPlayers) {
			if (player.id == newPlayer.id) {
				std::cout << "new player packet sent when we already have a player with that ID\n";
				break;
			}
			else {
				extPlayers.push_back(newPlayer);
			}
		}
	}
	else {
		extPlayers.push_back(newPlayer);
	}
	
}


int getPacketHeader(std::string packetData) {
	size_t delimiterPos = packetData.find(';');

	std::string headerString = packetData.substr(0, delimiterPos);

	return std::stoi(headerString);
}

void newConnectionAcknowledge(std::string rawPacketData) {
	std::cout << rawPacketData << "\n";
	size_t delimiterPos = rawPacketData.find(';');
	if (delimiterPos != std::string::npos) {
		std::string idString = rawPacketData.substr(delimiterPos + 1);
		player.id = std::stoi(idString);
		std::cout << "player id is now: " << player.id << "\n";
	} else {
		std::cout << "invalid acknowledgement packet\n";
	}
}


#pragma endregion