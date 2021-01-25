#pragma once
#include <SFML/Graphics.hpp>
#include "../../Utilities/Logger.h"

enum class TileType {
	WALL_CONNECTOR_NE, WALL_UP, WALL_DIAGONAL_NE, WALL_DIAGONAL_SE, WALL_DIAGONAL_SW, WALL_DIAGONAL_NW, WALL_CONNECTOR_SW,
	WALL_CONNECTOR_SE, WALL_CONNECTOR_NW, GROUND, SPIKES, LAVA, GROUND_SLIPPY, EMPTY, WALL_LEFT, WALF_DOWN, WALL_RIGHT, PLAYER_SPAWN = 23, ITEM_SPAWN
};

class Tile
{
	TileType type;
	sf::Sprite sprite;

public:
	Tile(TileType type, sf::Texture& tex);

	void draw(sf::RenderWindow& window, int x, int y);
};

