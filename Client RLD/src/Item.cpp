#include "Item.h"

Item::Item(ItemType type, int x, int y, sf::Texture& tex, bool onMap, bool inPocket) {
	this->sprite.setTexture(tex);
	this->type = type;
	this->x = x;
	this->y = y;
	this->onMap = onMap;
	this->inPocket = inPocket;
}

void Item::draw(sf::RenderWindow& window) {
	if (this->onMap){
		this->sprite.setPosition(this->x * Constants::SPRITE_WIDTH, this->y * Constants::SPRITE_HEIGHT);
		window.draw(this->sprite);
	}
}

void Item::drawInPocket(sf::RenderWindow& window, int posX, int posY) {
	if (this->inPocket) {
		this->sprite.setPosition(posX, posY);
		window.draw(this->sprite);
	}
}