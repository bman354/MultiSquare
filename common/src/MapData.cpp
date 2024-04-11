#include "MapData.h"



Tile& MapData::get(int x, int y) {
	if (x < 0 || y < 0 || x >= w || y >= h) {
		nullBlock = {};
		return nullBlock;
	}

	return data[x + this->w * y];

}

void MapData::render(gl2d::Renderer2D& renderer, gl2d::TextureAtlas& textureAtlas) {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
            renderMapTile(renderer, get(x, y), textureAtlas, x, y);
		}
	}
}

//load a map from csv file
bool MapData::load(const char* file) {
    std::ifstream inputFile(file);
    if (!inputFile.is_open()) {
        std::cout << "Unable to open file\n";
        return false;
    }

    int xMax = 0;
    int yMax = 0;

    std::string line;
    int y = 0;
    std::vector<unsigned short> data;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string token;
        int x = 0;        
        
        while (std::getline(iss, token, ',')) {
            data.push_back((unsigned short)std::stoi(token));
            x++;
            if (x > xMax) { xMax = x;}
        }
        y++;
        if (y > yMax) { yMax = y; }
    }

    create(yMax, xMax, &data[0]);
    inputFile.close();
    return true;
}

void MapData::create(int w, int h, const unsigned short* d) {
	this->w = w;
	this->h = h;
	data = new Tile[w * h];

    if (d) {
        for (int i = 0; i < w * h; i++) {
            data[i].type = d[i];
        }
    }
}
void MapData::cleanup() {
    if (data)
    {
        delete[] data;
        data = nullptr;
    }

    w = 0;
    h = 0;
}