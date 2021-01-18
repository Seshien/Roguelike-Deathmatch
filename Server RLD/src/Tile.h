#pragma once

enum class TileType {
	GROUND, GROUND_SLIPPY, LAVA, WALL_UP, WALL_LEFT, WALL_RIGHT, WALL_DOWN, WALL_DIAGONAL_SE, WALL_DIAGONAL_SW, WALL_DIAGONAL_NE,
	WALL_DIAGONAL_NW, WALL_CONNECTOR_SW, WALL_CONNECTOR_SE, WALL_CONNECTOR_NE, EMPTY
};

// Pozycja poszczeg�lnych Tili wynika z ich po�o�enia w Map, wi�c nie potrzebne jest zapami�tywanie ich koordynat x i y.

class Tile
{
	TileType type;
	int itemID;
	bool isItem;
	bool isPlayer;
	bool isSpawnable;
	int playerID;

public:
	Tile() {};
	Tile(TileType type);
};

