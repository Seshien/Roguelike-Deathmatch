#pragma once

enum class TileType {
	WALL_CONNECTOR_NE, WALL_UP, WALL_DIAGONAL_NE, WALL_DIAGONAL_SE, WALL_DIAGONAL_SW, WALL_DIAGONAL_NW, WALL_CONNECTOR_SW,
	WALL_CONNECTOR_SE, WALL_CONNECTOR_NW, GROUND, SPIKES, LAVA, GROUND_SLIPPY, EMPTY, WALL_LEFT, WALF_DOWN, WALL_RIGHT
};

// Pozycja poszczególnych Tili wynika z ich po³o¿enia w Map, wiêc nie potrzebne jest zapamiêtywanie ich koordynat x i y.

class Tile
{
	TileType type;

public:
	Tile() {};
	Tile(TileType type);
};

