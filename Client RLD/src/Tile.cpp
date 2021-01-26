#include "Tile.h"

Tile::Tile(TileType type, sf::Texture& tex) {
	this->type = type;
	this->sprite.setTexture(tex);
}

void Tile::draw(sf::RenderWindow& window, int x, int y) {
	//Logger::log(std::to_string(x) +  std::to_string(y));
	this->sprite.setPosition((float)x, (float)y);
	window.draw(this->sprite);
};