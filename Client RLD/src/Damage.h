#pragma once
#include <SFML/Graphics.hpp>

// Klasa odpowiada za wyswietlanie animacji dostania obrazen. Obiekty tego typu sa usuwane w kliencie, kiedy ich timer osiagnie wartosc 0.
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

