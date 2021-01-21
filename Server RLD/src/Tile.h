#pragma once
enum class TileType {
	WALL_CONNECTOR_NE, WALL_UP, WALL_DIAGONAL_NE, WALL_DIAGONAL_SE, WALL_DIAGONAL_SW, WALL_DIAGONAL_NW, WALL_CONNECTOR_SW,
	WALL_CONNECTOR_SE, WALL_CONNECTOR_NW, WALL_LEFT, WALL_DOWN, WALL_RIGHT, GROUND, SPIKES, LAVA, GROUND_SLIPPY, EMPTY
};

// Pozycja poszczególnych Tili wynika z ich położenia w Map, więc nie potrzebne jest zapamiętywanie ich koordynat x i y.

class Tile
{
private:
	int x;
	int y;
	TileType type;
	int itemID;
	bool isItem;
	bool isPlayer;
	bool isSpawnable;
	bool isMovable;
	int playerID;
public:
	Tile(TileType tileType, int x, int y) 
	{
		isItem = false;
		isPlayer = false;
		int itemID = -1;
		int playerID = -1;
		this->type = tileType;
		this->x = x;
		this->y = y;
		if ((int)tileType >= 12 && (int)tileType <= 16)
		{
			isSpawnable = true;
			isMovable = true;
		}
		else {
			isSpawnable = false;
			isMovable = false;
		}

	}
	bool havePlayer() { return isPlayer; };
	void setPlayer(bool value) { this->isPlayer = value; };

	bool haveItem() { return isItem; };
	void setItem(bool value) { this->isItem = value; };

	int getPlayerID() { return playerID; };
	void setPlayerID(int value) { this->playerID = value; };

	int getItemID() { return itemID; };
	void setItemID(int value) { this->itemID = value; };

	bool canSpawn() { return isSpawnable; };
	void setSpawn(bool value) { this->isSpawnable = value; };

	bool canMove() { return isMovable; };
	void setMove(bool value) { this->isMovable = value; };

	TileType getType() { return this->type; };

	int getX() { return this->x; };
	int getY() { return this->y; };
	//Tile(TileType type);
};

