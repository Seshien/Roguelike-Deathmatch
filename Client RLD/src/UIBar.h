#pragma once
#include <SFML/Graphics.hpp>

// Klasa odpowiedzialna za wyswietlanie paska zycia
class UIBar
{
	int xPos;
	int yPos;
	int width;
	int height;
	sf::Sprite sprite;
	bool visibility;
	double fillPercent;

public:
	UIBar() {};
	// Tworzy pasek zycia - fillPercent przyjmuje wartosc od 0 do 1 i jest przekazywano jako Hp/MaxHp
	UIBar(int x, int y, int width, int height, sf::Texture& tex, double fillPercent);

	void draw(sf::RenderWindow& window);

	void changeVisibility(bool visibility);

	void changeFillPercent(double fillPercent);
};

