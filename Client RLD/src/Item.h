#pragma once
#include <SFML/Graphics.hpp>
#include "..\..\Utilities\Config.h"

enum class ItemType { BOOTS = 17, POTION, SHIELD, BONES, SWORD };

// Klasa odpowiadajaca zarowno za wyswietlanie przedmiotow na mapie jak i w ekwipunku gracza. Enum startuje od 17, gdyz takie jest
// ulozenie sprite'ow przedmiotow w pliku tilesObjects.png

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

	// Rysuje przedmiot w przypadku, gdy onMap jest ustawiony na true.
	void draw(sf::RenderWindow& window);

	// Rysuje przedmiot w przypadku, gdy inPocket jest ustawiony na true.
	void drawInPocket(sf::RenderWindow& window, int posX, int posY);

	int getX();
	int getY();

	bool isOnMap();
	ItemType getType();

};

