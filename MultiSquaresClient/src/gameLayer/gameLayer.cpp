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
#include <tiledRenderer.h>
#include <cstdio>
#include <glui/glui.h>
#include <raudio.h>

#include "Bullet.h"
#include "Player.h"

std::vector<std::string> processPacket(ENetEvent* packetEvent);


//TODO Client state into int ENUM
/*
enum GAME_STATE{

	MAIN_MENU



}
*/


enum PacketHeader
{
	NEW_CONNECTION = 10,
	POSITION_UPDATE = 20,
	NEW_CONNECTION_ACKNOWLEDGE = 30,
	NEW_OUTSIDE_PLAYER_CONNECTED = 40,
	SYNC_UPDATE = 50
};

Player player;
std::vector<Bullet> allBullets;
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
//TODO swap this to a single state ENUM/Variable
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
				event = {};
				//get server connection
				if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
					std::cout << "Connection success!\n";
					IS_CONNECTED = true;

					player.name = playerMenuName;
					//we got a connection, send the init packet with player data and await starting info
					std::string playerData = player.newConnectionDataPacket();
					sendPacketReliable(std::to_string(NEW_CONNECTION) + ";" + playerData);

					bool ESTABLISHED_CONNECTION = false;
					while (!ESTABLISHED_CONNECTION) {
						rendererUi.Begin(02);
						rendererUi.Text("Connecting To Server...", Colors_Black);
						if (enet_host_service(client, &event, 0) > 0) {

							//check to see if header is NEW_CONNECTION_ACKNOWLEDGE and apply recieved ID to player
							std::vector<std::string> packetData = processPacket(&event);
							if (stoi(packetData[0]) == NEW_CONNECTION_ACKNOWLEDGE) {
								player.id = stoi(packetData[1]);
								player.pos.x = stoi(packetData[2]);
								player.pos.y = stoi(packetData[3]);
								ESTABLISHED_CONNECTION = true;
							}
						}

						rendererUi.End();
						renderer.clearScreen(Colors_White);
						rendererUi.renderFrame(renderer, font, platform::getRelMousePosition(), platform::isLMousePressed(), platform::isLMouseHeld(), platform::isLMouseReleased(),
							platform::isButtonReleased(platform::Button::Escape), platform::getTypedInput(), deltaTime);
						renderer.flush();
					}
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


			player.velocity += inputAcceleration;
			player.update(deltaTime);


			//positional networing, every set number of frames send our current position and velocity to allow other clients to simulate
			if (sendPosTimer >= 1) {
				sendPosTimer--;
			}
			else {
				sendPosUpdate();
				sendPosTimer = 30;
			}


#pragma endregion


#pragma region network event handler

			while (enet_host_service(client, &event, 0) > 0) {

				std::string rawPacket(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
				std::vector<std::string> packetData = breakPacket(rawPacket);
				int packetHeader = std::stoi(packetData[0]);

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
						case POSITION_UPDATE: {
							handlePosUpdate(&packetData);
							break;
						}
						case NEW_OUTSIDE_PLAYER_CONNECTED: {
							handleNewOutsidePlayer(&packetData);
						break;
						}
					}
					
					break;
				}
				}
			}
#pragma endregion			

#pragma region render
			for (const Player& player : extPlayers) {
				renderPlayer(renderer, player, playerTexture);
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
std::vector<std::string> processPacket(ENetEvent* packetEvent) {

	std::string rawPacket(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
	std::istringstream packetStream(rawPacket);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(packetStream, segment, ';'))
	{
		seglist.push_back(segment);
	}
	return seglist;
}

void sendPosUpdate() {
	//send position update and reset timer
	sendPacket(std::to_string(POSITION_UPDATE)
		+ ";" + std::to_string(player.id)
		+ ";" + std::to_string(player.pos.x)
		+ ";" + std::to_string(player.pos.y)
		+ ";" + std::to_string(player.velocity.x)
		+ ";" + std::to_string(player.velocity.y));
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

void handlePosUpdate(std::vector<std::string>* packetData) {

	for (Player &player : extPlayers) {
			std::cout << "player id " << packetData->at(1) << " updated\n";
		if (player.id == std::stoi(packetData->at(1))) {
			player.pos.x = std::stof(packetData->at(2));
			player.pos.y = std::stof(packetData->at(3));
			player.velocity.x = std::stof(packetData->at(4));
			player.velocity.y = std::stof(packetData->at(5));
			std::cout << "Successfully updated player data for ext player\n";
		}
	}
}

void handleNewOutsidePlayer(std::vector<std::string> *packetData) {

	Player newPlayer = {};

	newPlayer.id = std::stoi(packetData->at(1));
	newPlayer.pos.x = std::stof(packetData->at(2));
	newPlayer.pos.y = std::stof(packetData->at(3));
	newPlayer.velocity.x = std::stof(packetData->at(4));
	newPlayer.velocity.y = std::stof(packetData->at(5));
	newPlayer.health = std::stof(packetData->at(6));
	newPlayer.maxSpeed = std::stof(packetData->at(7));
	newPlayer.damage = std::stof(packetData->at(8));
	newPlayer.acceleration = std::stof(packetData->at(9));
	newPlayer.bulletSpeed = std::stof(packetData->at(10));
}


#pragma endregion