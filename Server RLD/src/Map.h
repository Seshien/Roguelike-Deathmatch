#pragma once
#include <array>

#include "../../Utilities/Logger.h"
#include "Tile.h"
#include "SpawnableObject.h"

class Map
{
public:
	static const int MAP_HEIGHT = 50;
	static const int MAP_WIDTH = 50;
	
public:

	enum class MOVEDIR { UP='W', DOWN='S', LEFT='A', RIGHT='D' };
	Map()
	{
	}
	void init(std::string path)
	{
		loadFromFile(std::string("data/map1.txt"));
	}
	void loadFromFile(std::string path)
	{
		Logger::log("Loading map from file");
		std::fstream file;
		std::string line;
		file.open(path);
		int j = 0;
		if (file.is_open()) {
			while (std::getline(file, line)) {
				for (size_t i = 0; i < line.size(); i += 2) {
					tileArray[i / 2][j] = std::make_shared<Tile>((TileType)(line[i] - 'A'), i/2 , j);
				}
				j++;
			}
			Logger::log("Map loading completed!");
		}
		else {
			Logger::log("Error. Map with filename " + path + "not found!");
		}
		file.close();
	}
	void refresh()
	{
		for (auto& arr : tileArray)
			for (auto& tile : arr)
			{
				if (tile == nullptr) break;
				tile = std::make_shared<Tile>(tile->getType(), tile->getX(), tile->getY());
			}
	}

	std::shared_ptr<Tile> getTile(int x, int y);
	bool checkRange(std::shared_ptr<Tile> objectF, std::shared_ptr<Tile> objectS, int range = Config::sightValue);
	
	std::array<std::array<std::shared_ptr<Tile>, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	std::vector<SpawnableObject> upperLayer;
	std::shared_ptr<Tile> getMovementTile(MOVEDIR movement, std::shared_ptr<Tile> tile, int range = 1);

};

