#pragma once
#include <SFML/Graphics.hpp>

// Klasa odpowiedzialna za wyswietlanie i sprawdzanie klikniec na dane przyciski w interfejsie.
class UIButton
{
	int xPos;
	int yPos;
	int width;
	int height;
	sf::Sprite sprite;
	std::string text;
	bool visibility;
public:
	UIButton() {};
	UIButton(int x, int y, int width, int height, std::string text, sf::Texture& tex);

	// Sprawdza czy klikniecie nastapilo w obszarze danego przycisku (jego visibility musi byc ustawione na true)
	bool isClickInBounds(int clickX, int clickY);

	void draw(sf::RenderWindow& window);

	void setText(std::string newText);

	void changeVisibility(bool visibility);
};

