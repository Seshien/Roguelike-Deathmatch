#pragma once
#include <SFML/Graphics.hpp>

enum class TileType {
	GROUND, GROUND_SLIPPY, LAVA, WALL_LEFT, WALL_UP, WALL_RIGHT, WALL_DOWN, WALL_DIAGONAL_SE, WALL_DIAGONAL_SW, WALL_DIAGONAL_NE,
	WALL_DIAGONAL_NW, WALL_CONNECTOR_SW, WALL_CONNECTOR_SE, WALL_CONNECTOR_NE, EMPTY, SPIKES
};

class Tile
{
	TileType type;
	sf::Sprite sprite;

public:
	Tile(TileType type, sf::Texture tex);

	void draw(sf::RenderWindow& window, int x, int y);
};

