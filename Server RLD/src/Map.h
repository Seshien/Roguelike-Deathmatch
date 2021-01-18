#pragma once
#include <array>

#include "../../Utilities/Logger.h"
#include "Tile.h"
#include "SpawnableObject.h"

class Map
{
	static const int MAP_HEIGHT = 100;
	static const int MAP_WIDTH = 100;
	
public:
	Map()
	{
	}
	void init(std::string path)
	{
		loadFromFile(path);
	}
	void loadFromFile(std::string path)
	{
		return;
	}
	std::array<std::array<Tile, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	std::vector<SpawnableObject> upperLayer;
};

