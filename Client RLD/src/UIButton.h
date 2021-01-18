#pragma once
#include <SFML/Graphics.hpp>

class UIButton
{
	int xPos;
	int yPos;
	int width;
	int height;
	sf::Sprite sprite;
	std::string text;
public:
	UIButton() {};
	UIButton(int x, int y, int width, int height, std::string text, sf::Texture tex);

	bool isClickInBounds(int clickX, int clickY);

	void draw(sf::RenderWindow& window);

};

