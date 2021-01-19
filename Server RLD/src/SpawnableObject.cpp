#include "SpawnableObject.h"

SpawnableObject::SpawnableObject(int itemID, SpawnableObjectType type, std::shared_ptr<Tile> tile) {
	this->type = type;
	this->isExist = false;
	this->occupiedTile = tile;
	this->occupiedTile->isItem = false;
	this->occupiedTile->itemID = itemID;
	this->itemID = itemID;
	this->x = tile->x;
	this->y = tile->y;
};

void SpawnableObject::spawn() 
{
	this->isExist = true;
	this->occupiedTile->isItem = true;
	this->occupiedTile->itemID = itemID;
}

void SpawnableObject::despawn() 
{
	this->isExist = false;
	this->occupiedTile->isItem = false;
	// Operacje na obiekcie map
}

tickResult SpawnableObject::tick()
{
	if (isExist)
	{
		if (timetoDespawn)
			timetoDespawn--;
		else
		{
			return tickResult::DESPAWNED;
		}
	}
	else
	{
		if (timetoRespawn)
			timetoRespawn--;
		else
		{
			return tickResult::SPAWNED;
		}

	}
	return tickResult::NOTHING;
}

void SpawnableObject::startSpawnTimer() {
	if (this->type == SpawnableObjectType::PLAYER) 
		this->timetoRespawn = Constants::PLAYER_RESPAWN_TIME;
	else if (this->type == SpawnableObjectType::BODY)
		this->timetoDespawn = Constants::PLAYER_RESPAWN_TIME;
	else
		this->timetoRespawn = Constants::ITEM_RESPAWN_TIME;
}

