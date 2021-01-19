#include "ItemObject.h"

ItemObject::ItemObject(int itemID, SpawnableObjectType type, std::shared_ptr<Tile> tile) {
	this->type = type;
	this->isExist = false;
	this->occupiedTile = tile;
	this->occupiedTile->isItem = false;
	this->occupiedTile->itemID = itemID;
	this->itemID = itemID;
	this->x = tile->x;
	this->y = tile->y;
};

void ItemObject::spawn()
{
	this->isExist = true;
	this->occupiedTile->isItem = true;
	this->occupiedTile->itemID = itemID;
}

void ItemObject::despawn()
{
	this->isExist = false;
	this->occupiedTile->isItem = false;
	// Operacje na obiekcie map
}

