#pragma once
#include <array>
#include <SFML/Graphics.hpp>
#include "../../Utilities/Logger.h"
#include "../../Utilities/Constants.h"
#include "Tile.h"


class Map
{
public:
	static const int MAP_HEIGHT = 100;
	static const int MAP_WIDTH = 100;

public:
	Map()
	{
	}
	void init(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex)
	{
		loadFromFile(path, tex);
	}
	void loadFromFile(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex)
	{
		Logger::log("Creating dummy map started");
		for (int i = 0; i < MAP_HEIGHT; i++)
			for (int j = 0; j < MAP_WIDTH; j++)
			{
				tileArray[j][i] = std::make_shared<Tile>(TileType::GROUND, *(tex[(int)TileType::GROUND]));
			}
		Logger::log("Creating dummy map completed");
		return;
	}
	std::array<std::array<std::shared_ptr<Tile>, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	//std::vector<SpawnableObject> upperLayer;

	void drawMap(sf::RenderWindow& window) {
		for (int j = 0; j <this->MAP_HEIGHT; j++) {
			for (int i = 0; i < this->MAP_WIDTH; i++) {
				this->tileArray[i][j]->draw(window, i * Constants::SPRITE_WIDTH, j * Constants::SPRITE_HEIGHT);
			}
		}
	}
};