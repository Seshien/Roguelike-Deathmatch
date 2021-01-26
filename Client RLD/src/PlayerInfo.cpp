#include "PlayerInfo.h"

PlayerInfo::PlayerInfo(std::string playerName, int x, int y, std::vector<std::shared_ptr<sf::Texture>> textureSet, int killCount) {
	this->isAlive = true;
	this->playerName = playerName;
	this->xPos = x;
	this->yPos = y;
	this->realXPos = x;
	this->realYPos = y;
	this->textureSet = textureSet;
	this->killCount = killCount;
}

std::string PlayerInfo::getPlayerName() {
	return this->playerName;
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

// Only for respawn purposes
void PlayerInfo::setPrevPosition(int x, int y) {
	this->prevxPos = x;
	this->prevyPos = y;
}

std::string PlayerInfo::getFacing() {
	// For respawn purposes only (0, 0) set at respawn!
	if (prevxPos == 0 && prevyPos == 0) {
		return std::string("down");
	}
	if (prevxPos < xPos) {
		return std::string("right");
	}
	else if (prevxPos > xPos) {
		return std::string("left");
	}
	else if (prevyPos > yPos) {
		return std::string("up");
	}
	else {
		return std::string("down");
	}
}

void PlayerInfo::draw(sf::RenderWindow& window) {
	if (this->isAlive) {
		std::string facing = this->getFacing();
		//Logger::log("Drawing character");
		this->sprite.setPosition(this->realXPos * Config::SPRITE_WIDTH, this->realYPos * Config::SPRITE_HEIGHT);
		if (facing == "left") {
			this->sprite.setTexture(*(this->textureSet[3]));
		}
		else if (facing == "up") {
			this->sprite.setTexture(*(this->textureSet[1]));
		}
		else if (facing == "right") {
			this->sprite.setTexture(*(this->textureSet[2]));
		}
		else {
			this->sprite.setTexture(*(this->textureSet[0]));
		}
		window.draw(this->sprite);
	}
}

void PlayerInfo::setIsAlive(bool isAlive) {
	this->isAlive = isAlive;
}

int PlayerInfo::getX() {
	return this->xPos;
}

int PlayerInfo::getY() {
	return this->yPos;
}

int PlayerInfo::getKillCount() {
	return this->killCount;
}

void PlayerInfo::setKillCount(int killNum) {
	this->killCount = killNum;
}