#pragma once
enum class TileType {
	WALL_CONNECTOR_NE, WALL_UP, WALL_DIAGONAL_NE, WALL_DIAGONAL_SE, WALL_DIAGONAL_SW, WALL_DIAGONAL_NW, WALL_CONNECTOR_SW,
	WALL_CONNECTOR_SE, WALL_CONNECTOR_NW, WALL_LEFT, WALL_DOWN, WALL_RIGHT, GROUND, SPIKES, LAVA, GROUND_SLIPPY, EMPTY
};

// Pozycja poszczególnych Tili wynika z ich po³o¿enia w Map, wiêc nie potrzebne jest zapamiêtywanie ich koordynat x i y.

class Tile
{
public:
	TileType type;

	int itemID;
	bool isItem;
	bool isPlayer;
	bool isSpawnable;
	bool isRuchAble;
	int playerID;
	int x;
	int y;
public:
	Tile(TileType tileType, int x, int y) 
	{
		isItem = false;
		isPlayer = false;
		int itemID = -1;
		int playerID = -1;
		this->type = tileType;
		if ((int)tileType >= 12 && (int)tileType <= 16)
		{
			isSpawnable = true;
			isRuchAble = true;
		}


	}
	//Tile(TileType type);
};

