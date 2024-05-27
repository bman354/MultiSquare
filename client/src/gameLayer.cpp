#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platform/platformInput.h"
//#include "imgui.h"
#include <iostream>
#include <sstream>
#include "Networker.h"
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

//TODO figure out a structure to hold all this data, this is too much just sitting around in an odd scope
//use one structure that is just players, "player" can just be an index or a key to the local player?

Player player;
std::vector<Bullet> bullets;
std::vector<Player> extPlayers;

MapData map = {};

gl2d::Renderer2D renderer;
glui::RendererUi rendererUi;

gl2d::Texture playerTexture;
gl2d::Texture bulletTexture;
gl2d::Texture menuTexture;

gl2d::Texture mapTexture;
gl2d::TextureAtlas mapAtlas;

gl2d::Font font;

char ipAddress[30];
char port[20];
char playerMenuName[30];

std::string newPlayerName = "Boos";
std::string ipToConnectTo = "localhost";
std::string portToConnectTo = "42040";

Networker networker;


glm::vec2 lastPlayerPos;
glm::vec2 deltaPos;



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
		player.name = playerMenuName;
		
		//set up connection
		if (!networker.status.connected) {
			networker.initNetworker(ipAddress, port, player, extPlayers);
			networker.doHandshake(player, extPlayers);
		}
		else 
		{

#pragma endregion

#pragma region movement input
			lastPlayerPos = player.pos;
			
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
			
			glm::vec2 currentDeltaPos = player.pos - lastPlayerPos;
			deltaPos += currentDeltaPos;

			float threshhold = 0.1f;
			if (deltaPos.length() > threshhold) {
				networker.sendPlayerPosUpdate(player);
				deltaPos.x = 0.0f;
				deltaPos.y = 0.0f;
			}

#pragma endregion
			//handle incoming network requests
			networker.doEnetEventService(player, extPlayers, bullets);


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
				networker.serverTime++;
#pragma endregion
			}
			return true;
		}
	}


//This function might not be be called if the program is forced closed
void closeGame(){
	networker.closeNetworker();

}

glm::vec2 getMouseDirection(float w, float h) {

	glm::vec2 mousePos = platform::getRelMousePosition();
	glm::vec2 screenCenter(w / 2.f, h / 2.f);

	return mousePos - screenCenter;
}