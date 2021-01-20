#include "OurPlayerInfo.h"

OurPlayerInfo::OurPlayerInfo(std::string playerName , int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, int killCount):PlayerInfo(playerName, x, y, textureSet, killCount) {
	this->pocket = pocket;
	this->health = health;
	this->maxHealth = maxHealth;
}

void OurPlayerInfo::drawPocket(sf::RenderWindow& window, int posX, int posY) {
	for (int i = 0; i < this->pocket.size(); i++) {
		(*this->pocket[0]).drawInPocket(window, posX, posY);
	}
}