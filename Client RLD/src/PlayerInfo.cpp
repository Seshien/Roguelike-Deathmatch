#include "PlayerInfo.h"

PlayerInfo::PlayerInfo(int playerId, int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, int killCount) {
	this->isAlive = true;
	this->playerId = playerId;
	this->xPos = x;
	this->yPos = y;
	this->realXPos = x;
	this->realYPos = y;
	this->textureSet = textureSet;
	this->killCount = killCount;
}

void PlayerInfo::interpolate(double time, double maxTime) {
	this->realXPos = (double)this->prevxPos + ((double)this->xPos - (double)this->prevxPos) * (time / maxTime);
	this->realYPos = (double)this->prevyPos + ((double)this->yPos - (double)this->prevyPos) * (time / maxTime);
}

void PlayerInfo::setNewPosition(int x, int y) {
	this->prevxPos = this->xPos;
	this->prevyPos = this->yPos;
	this->xPos = x;
	this->yPos = y;
}

std::string PlayerInfo::getFacing() {
	if (prevxPos < xPos) {
		return std::string("right");
	}
	else if (prevxPos > xPos) {
		return std::string("left");
	}
	else if (prevyPos < yPos) {
		return std::string("up");
	}
	else {
		return std::string("down");
	}
}

void PlayerInfo::draw(sf::RenderWindow& window) {
	if (this->isAlive) {
		std::string facing = this->getFacing();
		if (facing == "down") {
			
		}
		else if (facing == "up") {

		}
		else if (facing == "right") {

		}
		else {

		}
		/*sf::RectangleShape rectangle(sf::Vector2f(304.0f * this->fillPercent / 100.0f, 48));
		rectangle.setPosition(this->xPos + 8, this->xPos + 8);
		rectangle.setFillColor(sf::Color(255, 0, 0));
		window.draw(rectangle);
		this->sprite.setPosition(sf::Vector2f(this->xPos, this->yPos));
		window.draw(this->sprite);*/
	}
}
