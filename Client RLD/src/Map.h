#pragma once
#include <array>

#include "../../Utilities/Logger.h"
#include "Tile.h"

class Map
{
public:
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
		Logger::log("Creating dummy map started");
		for (int i = 0; i < MAP_HEIGHT; i++)
			for (int j = 0; j < MAP_WIDTH; j++)
			{
				tileArray[j][i] = std::make_shared<Tile>(TileType::GROUND, j, i);
			}
		Logger::log("Creating dummy map completed");
		return;
	}
	std::array<std::array<std::shared_ptr<Tile>, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	//std::vector<SpawnableObject> upperLayer;
};