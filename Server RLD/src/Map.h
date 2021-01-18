#pragma once
#include <array>

#include "../../Utilities/Logger.h"
#include "Tile.h"
#include "SpawnableObject.h"

class Map
{
	static const int MAP_HEIGHT = 100;
	static const int MAP_WIDTH = 100;
	int mapID;
	
public:
	Map()
	{
	}
	void init(std::string path, int mapID)
	{
		this->mapID = mapID;
		loadFromFile(path);
	}
	void loadFromFile(std::string path)
	{
		return;
	}
	int getMapID() {
		return this->mapID;
	}
	std::array<std::array<Tile, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	std::vector<SpawnableObject> upperLayer;
};

