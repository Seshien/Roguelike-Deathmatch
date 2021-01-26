#pragma once
#include <array>
#include <SFML/Graphics.hpp>
#include "../../Utilities/Logger.h"
#include "../../Utilities/Config.h"
#include "Tile.h"
#include <fstream>


// Klasa odpowiadajaca za wyswietlana mape, w przypadku jej rysowania wyswietla wszystkie Tile znajdujace sie w tileArray.

class Map
{
public:
	static const int MAP_HEIGHT = 50;
	static const int MAP_WIDTH = 50;

public:
	Map();
	void init(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex);
	void loadFromFile(std::string path, std::vector<std::shared_ptr<sf::Texture>> tex);

	std::array<std::array<std::shared_ptr<Tile>, MAP_HEIGHT>, MAP_WIDTH> tileArray;
	//std::vector<SpawnableObject> upperLayer;

	void drawMap(sf::RenderWindow& window);
};