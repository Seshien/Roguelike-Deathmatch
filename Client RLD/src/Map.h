#pragma once
#include <array>
#include <SFML/Graphics.hpp>
#include "../../Utilities/Logger.h"
#include "../../Utilities/Config.h"
#include "Tile.h"
#include <fstream>


class Map
{
public:
	static const int MAP_HEIGHT = 50;
	static const int MAP_WIDTH = 50;

public:
	Map()
	{
	}
	void init(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex)
	{
		loadFromFile(std::string("data/map1.txt") , tex);
	}
	void loadFromFile(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex)
	{
		Logger::log("Loading map from file");
		std::fstream file;
		std::string line;
		file.open(path);
		int j = 0;
		if (file.is_open()) {
			while (std::getline(file, line)) {
				for (int i = 0; i < line.size(); i+=2) {
					tileArray[i/2][j] = std::make_shared<Tile>((TileType)(line[i] - 'A'), *tex[line[i] - 'A']);
				}
				j++;
			}
			Logger::log("Map loading completed!");
		}
		else {
			Logger::log("Error. Map with filename " + path + "not found!");
		}
		file.close();
	}
	std::array<std::array<std::shared_ptr<Tile>, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	//std::vector<SpawnableObject> upperLayer;

	void drawMap(sf::RenderWindow& window) {
		for (int j = 0; j <this->MAP_HEIGHT; j++) {
			for (int i = 0; i < this->MAP_WIDTH; i++) {
				this->tileArray[i][j]->draw(window, i * Config::SPRITE_WIDTH, j * Config::SPRITE_HEIGHT);
			}
		}
	}
};