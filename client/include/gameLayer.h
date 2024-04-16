#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <vector>


bool initGame();
bool gameLogic(float deltaTime);
void closeGame();
void sendPacket(std::string data);
void sendPacketReliable(std::string data);
void sendPosUpdate();
void playerUpdate(std::string newData);
int getPacketHeader(std::string packetData);
void newPlayerConnected(std::string packetData);
void newConnectionAcknowledge(std::string rawPacketData);
glm::vec2 getMouseDirection(float w, float h);


namespace platform
{
	///sets the mouse pos relative to the window's drawing area
	void setRelMousePosition(int x, int y);

	bool isFullScreen();
	void setFullScreen(bool f);

	//gets the window size, can be different to the actual framebuffer size
	//you should use getFrameBufferSize if you want the actual ammount of pixels to give to your opengl routines
	glm::ivec2 getWindowSize();

	inline int getWindowSizeX() { return getWindowSize().x; }
	inline int getWindowSizeY() { return getWindowSize().y; }

	//usually is the same as getWindowSize unless you have things like zoom or rezolution.
	//You should use this function if you want to pass this data to glviewport
	glm::ivec2 getFrameBufferSize();

	inline int getFrameBufferSizeX() { return getFrameBufferSize().x; }
	inline int getFrameBufferSizeY() { return getFrameBufferSize().y; }


	///gets the mouse pos relative to the window's drawing area
	glm::ivec2 getRelMousePosition();

	void showMouse(bool show);
	bool isFocused();
	bool mouseMoved();

	bool writeEntireFile(const char* name, void* buffer, size_t size);
	bool readEntireFile(const char* name, void* buffer, size_t size);

};