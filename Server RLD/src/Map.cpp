#include "Map.h"


std::shared_ptr<Tile> Map::getTile(int x, int y)
{
	if (x >= 0 && x < this->MAP_WIDTH && y >= 0 && y < this->MAP_HEIGHT)
		return this->tileArray[x][y];
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

std::shared_ptr<Tile> Map::getMovementTile(MOVEDIR movement, std::shared_ptr<Tile> tile, int range)
{
	int x = tile->getX();
	int y = tile->getY();
	switch (movement)
	{
	case MOVEDIR::UP:
		return getTile(x, y - range);
		break;
	case MOVEDIR::DOWN:
		return getTile(x, y + range);
		break;
	case MOVEDIR::LEFT:
		return getTile(x - range, y);
		break;
	case MOVEDIR::RIGHT:
		return getTile(x + range, y);
		break;
	default:
		return getTile(x, y - range);
	}
}