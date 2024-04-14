#pragma once
#include <gl2d/gl2d.h>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>


struct Tile {
	unsigned short type;
	bool collidable;
};

struct MapData {

	Tile nullBlock = {};
	Tile* data;

	int w;
	int h;

	void create(int w, int h, const unsigned short* d);
	Tile& get(int x, int y);

	void toConsole();
	void cleanup();

	bool load(const char* file);
};


