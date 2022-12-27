#include "Map.h"


Map::Map()
{

}

void Map::init(std::string path)
{
	loadFromFile(std::string("data/map1.txt"));
}

void Map::loadFromFile(std::string path)
{
	Logger::debug("Loading map from file");
	std::fstream file;
	std::string line;
	file.open(path);
	int j = 0;
	if (file.is_open()) {
		while (std::getline(file, line)) {
			for (size_t i = 0; i < line.size(); i += 2)
				tileArray.push_back(std::make_shared<Tile>((TileType)(line[i] - 'A'), i / 2, j));
			j++;
		}
		Logger::debug("Map loading completed!");
	}
	else {
		Logger::error("Error. Map with filename " + path + "not found!");
	}
	file.close();
}

void Map::refresh()
{
	for (auto& tile : tileArray)
	{
		if (tile == nullptr) break;
		tile = std::make_shared<Tile>(tile->getType(), tile->getX(), tile->getY());
	}
}


std::shared_ptr<Tile> Map::getTile(int xy)
{
	if (xy >= 0 && xy < this->MAP_WIDTH*this->MAP_HEIGHT )
		return this->tileArray[xy];
	else
		return std::shared_ptr<Tile>(nullptr);

}


std::shared_ptr<Tile> Map::getTile(int x, int y)
{
	if (x >= 0 && x < this->MAP_WIDTH && y >= 0 && y < this->MAP_HEIGHT)
		return this->tileArray[x+y*this->MAP_WIDTH];
	else
		return std::shared_ptr<Tile>(nullptr);

}

bool Map::checkRange(std::shared_ptr<Tile> objectF, std::shared_ptr<Tile> objectS, int range)
{
	int objX = objectF->getX();
	int objY = objectF->getY();
	int x1 = objectS->getX() - range;
	int y1 = objectS->getY() - range;
	int x2 = x1 + range * 2;
	int y2 = y1 + range * 2;
	return objX > x1 && objX < x2 && objY > y1 && objY < y2;
}

std::shared_ptr<Tile> Map::getNextTile(MOVEDIR movement, std::shared_ptr<Tile> tile, int range)
{
	int x = tile->getX();
	int y = tile->getY();
	return getTile(x + y * this->MAP_WIDTH + transformMD(movement) * range);
}

int Map::transformMD(MOVEDIR movement)
{
	switch (movement)
	{
	case MOVEDIR::UP:
		return -1 * this->MAP_WIDTH;
	case MOVEDIR::DOWN:
		return this->MAP_WIDTH;
		break;
	case MOVEDIR::LEFT:
		return -1;
		break;
	case MOVEDIR::RIGHT:
		return 1;
		break;
	default:
		return 0;
	}
}

std::vector<std::shared_ptr<Tile>> Map::getAllTiles(int xOrigin, int yOrigin, int xRange, int yRange)
{
	bool xRising = true;
	bool yRising = true;
	int xIter = 1;
	int yIter = 1;
	std::vector<std::shared_ptr<Tile>> tiles;
	int xEnd = xOrigin + xRange;
	int yEnd = yOrigin + yRange;
	if (xEnd < xOrigin)
	{
		xRising = false;
		xIter = -1;
	}

	if (yEnd < yOrigin)
	{
		yRising = false;
		yIter = -1;
	}

	for (int x = xOrigin; x != xEnd + xIter; x += xIter)
	{
		for (int y = yOrigin; y != yEnd + yIter; y += yIter)
		{
			tiles.push_back(this->getTile(x, y));
		}
	}
	return tiles;
}
std::vector<std::shared_ptr<Tile>> Map::getMovableTiles(int xOrigin, int yOrigin, int xRange, int yRange)
{
	bool xRising = true;
	bool yRising = true;
	int xIter = 1;
	int yIter = 1;
	std::vector<std::shared_ptr<Tile>> tiles;
	int xEnd = xOrigin + xRange;
	int yEnd = yOrigin + yRange;
	if (xEnd < xOrigin)
	{
		xRising = false;
		xIter = -1;
	}

	if (yEnd < yOrigin)
	{
		yRising = false;
		yIter = -1;
	}

	for (int x = xOrigin; x != xEnd + xIter; x += xIter)
	{
		for (int y = yOrigin; y != yEnd + yIter; y += yIter)
		{
			auto tile = this->getTile(x, y);
			if (tile->isSolid())
			{
				if (x == xOrigin) break;
			}
			if (tile->canMove())
				tiles.push_back(tile);
		}
	}
	return tiles;
}