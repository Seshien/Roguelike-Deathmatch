#pragma once
#include <SFML/Graphics.hpp>
#include "..\..\Utilities\Constants.h"

enum class ItemType { BOOTS = 17, POTION, SHIELD, BONES, SWORD };

class Item
{
	ItemType type;
	int x;
	int y;
	sf::Sprite sprite;
	bool onMap;
	bool inPocket;

public:
	Item(ItemType type, int x, int y, sf::Texture& tex, bool onMap, bool inPocket);

	void draw(sf::RenderWindow& window);

	void drawInPocket(sf::RenderWindow& window, int posX, int posY);
};

