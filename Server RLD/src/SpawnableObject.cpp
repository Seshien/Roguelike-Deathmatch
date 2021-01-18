#include "SpawnableObject.h"

SpawnableObject::SpawnableObject(SpawnableObjectType type, int x, int y) {
	this->type = type;
	this->isAlive = false;
	this->x = x;
	this->y = y;
};

void SpawnableObject::spawn() {
	this->isAlive = true;
	// Operacje na obiekcie map
};

void SpawnableObject::decreaseRespawnTime() {
	if (!isAlive) {
		this->timetoRespawn -= 1;
		if (this->timetoRespawn == 0) {
			spawn();
		}
	}
};

void SpawnableObject::startRespawnTimer() {
	if (this->type == SpawnableObjectType::PLAYER) {
		this->timetoRespawn = Constants::PLAYER_RESPAWN_TIME;
	}
	else {
		this->timetoRespawn = Constants::ITEM_RESPAWN_TIME;
	}
}

void SpawnableObject::setSpawnCoords(int x, int y) {
	this->x = x;
	this->y = y;
}
