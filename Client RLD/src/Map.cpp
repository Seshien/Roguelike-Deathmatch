#include "Map.h"

Map::Map()
{
}

void Map::init(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex)
{
	loadFromFile(std::string("data/map1.txt"), tex);
}

void Map::loadFromFile(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex)
{
	Logger::info("Loading map from file");
	std::fstream file;
	std::string line;
	file.open(path);
	int j = 0;
	if (file.is_open()) {
		while (std::getline(file, line)) {
			for (int i = 0; i < line.size(); i += 2) {
				tileArray[i / 2][j] = std::make_shared<Tile>((TileType)(line[i] - 'A'), *tex[line[i] - 'A']);
			}
			j++;
		}
		Logger::info("Map loading completed!");
	}
	else {
		Logger::error("Error. Map with filename " + path + "not found!");
	}
	file.close();
}

void Map::drawMap(sf::RenderWindow& window) {
	for (int j = 0; j < this->MAP_HEIGHT; j++) {
		for (int i = 0; i < this->MAP_WIDTH; i++) {
			this->tileArray[i][j]->draw(window, i * Config::SPRITE_WIDTH, j * Config::SPRITE_HEIGHT);
		}
	}
}
