#pragma once
#include <SFML/Graphics.hpp>
#include "..\..\Utilities\Constants.h"
class PlayerInfo
{
	int playerId;
	int prevxPos;
	int prevyPos;
	int xPos;
	int yPos;
	int killCount;

	// Dla animowania/interpolacji w kliencie pomiedzy ruchami
	double realXPos;
	double realYPos;

	bool isAlive;

	std::vector<std::shared_ptr<sf::Texture>> textureSet;
	sf::Sprite sprite;

public:
	PlayerInfo(int playerId, int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, int killCount);
	// Sets realXPos and realYPos for given frame 
	void interpolate(double time, double maxTime);
	void setNewPosition(int x, int y);
	void draw(sf::RenderWindow& window);
	std::string getFacing();
};

