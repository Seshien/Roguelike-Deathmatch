#pragma once
#include <array>

#include "Logger.h"
class Map
{
	static const int MAP_HEIGHT = 100;
	static const int MAP_WIDTH = 100;

	struct Tile
	{

	};
	
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
};

