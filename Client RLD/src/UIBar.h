#pragma once
#include <SFML/Graphics.hpp>

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
	UIBar(int x, int y, int width, int height, sf::Texture& tex, double fillPercent);

	void draw(sf::RenderWindow& window);

	void changeVisibility(bool visibility);

	void changeFillPercent(double fillPercent);
};

