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

	Map();

	void init(std::string path);

	void loadFromFile(std::string path);

	void refresh();

	std::shared_ptr<Tile> getTile(int x, int y);
	std::shared_ptr<Tile> getTile(int xy);
	std::vector<std::shared_ptr<Tile>> getAllTiles(int xOrigin, int yOrigin, int xRange, int yRange);

	bool checkRange(std::shared_ptr<Tile> objectF, std::shared_ptr<Tile> objectS, int range = Config::getConfigHandler()->sightValue);
	
	//std::array<std::array<std::shared_ptr<Tile>, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	std::vector<std::shared_ptr<Tile>> tileArray;
	std::vector<SpawnableObject> upperLayer;
	std::shared_ptr<Tile> getNextTile(MOVEDIR movement, std::shared_ptr<Tile> tile, int range = 1);
	std::vector<std::shared_ptr<Tile>> getMovableTiles(int xOrigin, int yOrigin, int xRange, int yRange);

	int transformMD(MOVEDIR movement);
};

