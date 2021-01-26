#include "UIButton.h"

UIButton::UIButton(int x, int y, int width, int height, std::string text, sf::Texture& tex) {
	this->xPos = x;
	this->yPos = y;
	this->width = width;
	this->height = height;
	this->text = text;
	this->sprite.setTexture(tex);
	this->visibility = false;
}

bool UIButton::isClickInBounds(int clickX, int clickY) {
	if (!visibility) { return false; }
	return clickX >= this->xPos && clickX < this->xPos + this->width && clickY >= this->yPos && clickY < this->yPos + this->height;
}

void UIButton::draw(sf::RenderWindow& window) {
	//this->sprite.scale(sf::Vector2f(2.0f, 2.0f));
	if (this->visibility) {
		this->sprite.setPosition(sf::Vector2f((float)this->xPos, (float)this->yPos));
		window.draw(this->sprite);
		sf::Text buttonText;
		sf::Font font;
		font.loadFromFile("data/arial2.ttf");
		buttonText.setFont(font);
		buttonText.setString(this->text);
		buttonText.setFillColor(sf::Color(0, 0, 0, 255));
		buttonText.setPosition((float)this->xPos + 10, (float)this->yPos + 10);
		window.draw(buttonText);
	}
}

void UIButton::setText(std::string newText) {
	this->text = newText;
}

void UIButton::changeVisibility(bool visibility) {
	this->visibility = visibility;
}