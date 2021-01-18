#pragma once

enum class TileType {
	GROUND, GROUND_SLIPPY, LAVA, WALL_UP, WALL_LEFT, WALL_RIGHT, WALL_DOWN, WALL_DIAGONAL_SE, WALL_DIAGONAL_SW, WALL_DIAGONAL_NE,
	WALL_DIAGONAL_NW, WALL_CONNECTOR_SW, WALL_CONNECTOR_SE, WALL_CONNECTOR_NE, EMPTY
};

// Pozycja poszczególnych Tili wynika z ich położenia w Map, więc nie potrzebne jest zapamiętywanie ich koordynat x i y.

class Tile
{
	TileType type;

public:
	Tile() {};
	Tile(TileType type);
};

