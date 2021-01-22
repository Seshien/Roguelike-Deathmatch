#pragma once
#include <SFML/Graphics.hpp>

class Damage
{
	int x;
	int y;
	std::vector<std::shared_ptr<sf::Texture>> textureSet;
	sf::Sprite sprite;
	double timer;
	
public:
	Damage(int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, double timer);
	void decreaseTimer();
	void draw(sf::RenderWindow& window);
	double getTimer();
};

