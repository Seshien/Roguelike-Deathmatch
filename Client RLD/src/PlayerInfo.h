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
	std::string getPlayerName();
	// Ustawia realXPos i realYPos, aktualnie ta funkcja od razu przenosi gracza na dana kratke, nie ma plynnego ruchu
	void interpolate(double time, double maxTime);
	
	void setNewPosition(int x, int y);

	// W konstruktorze prevPositions s¹ ustawiane automatycznie, ta procedura jest wywolywana tylko jezeli gracz dostal event respawn
	void setPrevPosition(int x, int y);
	
	void draw(sf::RenderWindow& window);

	// Zwraca kierunek w jakim odwrocony jest gracz na podstawie jego poprzedniej pozycji i aktualnej
	std::string getFacing();

	void setIsAlive(bool isAlive);
	int getX();
	int getY();
	int getKillCount();
	bool getIsAlive();
	void setKillCount(int killNum);
};

