#include "Tile.h";

Tile::Tile(TileType type, sf::Texture& tex) {
	this->type;
	this->sprite.setTexture(tex);
}

void Tile::draw(sf::RenderWindow& window, int x, int y) {
	window.draw(this->sprite);
};