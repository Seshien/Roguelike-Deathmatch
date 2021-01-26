#include "UIBar.h"

UIBar::UIBar(int x, int y, int width, int height, sf::Texture& tex, double fillPercent) {
	this->xPos = x;
	this->yPos = y;
	this->width = width;
	this->height = height;
	this->sprite.setTexture(tex);
	this->visibility = false;
	this->fillPercent = fillPercent;
}

void UIBar::changeVisibility(bool visibility) {
	this->visibility = visibility;
}

void UIBar::draw(sf::RenderWindow& window) {
	if (this->visibility) {
		sf::RectangleShape rectangle(sf::Vector2f((float)(304.0f * this->fillPercent / 100.0f), (float)48));
		rectangle.setPosition((float)this->xPos + 8, (float)this->xPos + 8);
		rectangle.setFillColor(sf::Color(255, 0, 0));
		window.draw(rectangle);
		this->sprite.setPosition(sf::Vector2f((float)this->xPos, (float)this->yPos));
		window.draw(this->sprite);
	}
}

void UIBar::changeFillPercent(double fillPercent) {
	this->fillPercent = fillPercent;
}
