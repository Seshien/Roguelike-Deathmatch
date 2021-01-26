#include "Damage.h"

Damage::Damage(int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, double timer) {
	this->timer = timer;
	this->x = x;
	this->y = y;
	this->textureSet = textureSet;
}

void Damage::decreaseTimer() {
	this->timer = this->timer - 0.25f;
}

void Damage::draw(sf::RenderWindow& window) {
	this->sprite.setTexture(*(this->textureSet[3 - (int)this->timer]));
	this->sprite.setPosition((float)x, (float)y);
	window.draw(this->sprite);
}

double Damage::getTimer() {
	return this->timer;
}
