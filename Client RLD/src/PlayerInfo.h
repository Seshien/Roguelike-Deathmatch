#pragma once
#include <SFML/Graphics.hpp>
#include "../../Utilities/Config.h"
#include "../../Utilities/Logger.h"
class PlayerInfo
{
	std::string playerName;
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
	PlayerInfo(std::string playerName, int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, int killCount);
	// Sets realXPos and realYPos for given frame 
	std::string getPlayerName();
	void interpolate(double time, double maxTime);
	void setNewPosition(int x, int y);
	void draw(sf::RenderWindow& window);
	std::string getFacing();
	void setIsAlive(bool isAlive);
	int getX();
	int getY();
	int getKillCount();
	void setKillCount(int killNum);
};

