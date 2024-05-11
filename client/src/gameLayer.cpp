#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platform/platformInput.h"
//#include "imgui.h"
#include <iostream>
#include <sstream>
#include <enet/enet.h>
//#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platform/platformTools.h>
#include <renderer.h>
#include <cstdio>
#include <glui/glui.h>
#include <raudio.h>

#include "Bullet.h"
#include "Player.h"
#include "Packet.h"

void newPlayerConnected(NewPlayerConnectedPacket& newConnectedPacket);


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

gl2d::Texture mapTexture;
gl2d::TextureAtlas mapAtlas;


gl2d::Font font;

ENetHost* client = nullptr;
ENetPeer* server = nullptr;
ENetAddress address = {};
ENetEvent event = {};

char ipAddress[30];
char port[20];
char playerMenuName[30];

std::string newPlayerName = "Boos";
std::string ipToConnectTo = "localhost";
std::string portToConnectTo = "42040";

//GAMEPLAY FLAGS~~~~~~
//TODO swap this to a single state ENUM/Variable so I can use a switch statement instead of if/else
bool IN_MAIN_MENU = true;
bool IN_GAME = false;
bool IS_CONNECTED = false;
int selected = 0;

MapData map = {};
unsigned long long serverTime = 0;

bool initGame() {
	//creates the renderer and starts game
	gl2d::init();
	renderer.create();

	playerTexture.loadFromFile(RESOURCES_PATH "playerSkins/default.png");
	bulletTexture.loadFromFile(RESOURCES_PATH "bulletSkins/default.png");
	menuTexture.loadFromFile(RESOURCES_PATH "playerSkins/default.png");
	font.createFromFile(RESOURCES_PATH "Arial.ttf");

	mapTexture.loadFromFile(RESOURCES_PATH "mapTiles.png");
	mapAtlas = gl2d::TextureAtlas(14, 14);
	
	
	map.load("../common/resources/maps/TestMap.csv");

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

		player.name = playerMenuName;
		if (enet_initialize() != 0) {
			std::cout << "Enet failed to initialize\n";
			IN_MAIN_MENU = true;
			IS_CONNECTED = false;
		}

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
				//Enet Handshake
				if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
					std::cout << "Connection success!\n";
					IS_CONNECTED = true;
				}

				//Gamelayer Handshake
				Packet packet;
				packet.header = HANDSHAKE;

				HandshakePacket packetData;
				packetData.player = player;

				std::cout << "sending handshake\n";
				sendPacket(server, packet, (char*)&packetData, sizeof(packetData), true, 1);

				if (enet_host_service(client, &event, 5000) > 0) {
					packet = {};
					size_t dataSize = 0;

					auto data = parsePacket(event, packet, dataSize);
					if (packet.header == HANDSHAKE_CONFIRM) {
						HandshakeConfirmationPacket handshakeConfirmationPacket = *(HandshakeConfirmationPacket*)data;
						player.id = handshakeConfirmationPacket.id;
						std::cout << "new id: " << player.id << "\n";
						IS_CONNECTED = true;
					}
					else {
						std::cerr << "bad handshake\n";
						IS_CONNECTED = false;
						IN_MAIN_MENU = true;
					}
				}
			}
		}
		else {
#pragma endregion


#pragma region movement input

			glm::vec2 currentMouseDirection = getMouseDirection(w, h);

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

			//fire boolet
			if (platform::isLMousePressed()) {
				Bullet firedBullet(player, currentMouseDirection);
				bullets.push_back(firedBullet);
			}


			//boost time babyy
			if (player.boostTimer > 0) {
				player.boostTimer--;
			}
			else if (platform::isRMousePressed()) {
				player.boost(currentMouseDirection);
				player.boostTimer = player.maxBoostTimer;
			}



			player.velocity += inputAcceleration;
			player.update(deltaTime);


#pragma endregion


#pragma region network event handler
			//HACK probably needs to either go to another thread, ENET may do this already
			while (enet_host_service(client, &event, 0) > 0) {

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
					
					Packet packet;
					size_t dataSize = 0;

					auto data = parsePacket(event, packet, dataSize);


					switch (packet.header) {
						case NEW_PLAYER_CONNECTED: {
							NewPlayerConnectedPacket newConnectedPacket = *(NewPlayerConnectedPacket*)data;
							newPlayerConnected(newConnectedPacket);
							break;
						}
					}

					
					break;
				}
				
				}
#pragma endregion			

#pragma region rendering

				renderer.currentCamera.follow({ player.pos.x, player.pos.y }, 1.0f, 10.0f, 10.0f, w, h);
				renderer.currentCamera.zoom = 1.5f;

				renderMap(renderer, mapAtlas, mapTexture, map);

				for (Bullet& bullet : bullets) {
					bullet.update(deltaTime);
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
				serverTime++;
#pragma endregion
			}
			return true;
		}
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

glm::vec2 getMouseDirection(float w, float h) {

	glm::vec2 mousePos = platform::getRelMousePosition();
	glm::vec2 screenCenter(w / 2.f, h / 2.f);

	return mousePos - screenCenter;
}

void newPlayerConnected(NewPlayerConnectedPacket& newConnectedPacket) {
	extPlayers.push_back(newConnectedPacket.connectingPlayer);
	std::cout << newConnectedPacket.connectingPlayer.name << " has joined the game\n";
}
