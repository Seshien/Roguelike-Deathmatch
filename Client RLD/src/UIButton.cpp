#include "UIButton.h"

UIButton::UIButton(int x, int y, int width, int height, std::string text, sf::Texture tex) {
	this->xPos = x;
	this->yPos = y;
	this->width = width;
	this->height = height;
	this->text = text;
	this->sprite.setTexture(tex);
}

bool UIButton::isClickInBounds(int clickX, int clickY) {
	return clickX >= this->xPos && clickX < this->xPos + this->width && clickY >= this->yPos && clickY < this->yPos + this->height;
}

void UIButton::draw(sf::RenderWindow& window) {
	window.draw(this->sprite);
	sf::Text buttonText;
	sf::Font font;
	font.loadFromFile("data/arial2.ttf");
	buttonText.setFont(font);
	buttonText.setString(this->text);
	buttonText.setPosition(this->xPos, this->yPos);
	window.draw(buttonText);
}