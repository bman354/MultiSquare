#pragma once
#include <gl2d/gl2d.h>
#include "../../client/include/renderer.h"
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
	void render(gl2d::Renderer2D& renderer, gl2d::TextureAtlas& textureAtlas);

	void cleanup();

	bool load(const char* file);
};


