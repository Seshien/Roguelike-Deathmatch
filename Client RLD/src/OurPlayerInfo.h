#pragma once
#include "PlayerInfo.h"
#include "Item.h"
#include "SFML/Graphics.hpp"

class OurPlayerInfo : public PlayerInfo
{
public:
	int health;
	int maxHealth;

	std::vector<std::shared_ptr<Item>> pocket;

	// Funkcja, ktora rysuje posiadane przez gracza przedmioty w UI
	OurPlayerInfo(std::string playerName, int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, int killCount);
	void drawPocket(sf::RenderWindow& window, int posX, int posY);
};

