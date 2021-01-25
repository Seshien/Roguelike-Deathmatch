#include "ItemObject.h"

ItemObject::ItemObject(int itemID, SpawnableObjectType type, std::shared_ptr<Tile> tile) {
	this->type = type;
	this->isExist = false;
	this->itemID = itemID;
	this->occupiedTile = tile;
	this->occupiedTile->setItem(false);
	this->occupiedTile->setItemID(itemID);
	this->occupiedTile->setSpawn(false);
	this->x = tile->getX();
	this->y = tile->getY();
};

void ItemObject::spawn()
{
	this->isExist = true;
	this->occupiedTile->setItem(true);
	this->occupiedTile->setItemID(itemID);
}  

void ItemObject::despawn()
{
	this->isExist = false;
	this->occupiedTile->setItem(false);
	// Operacje na obiekcie map
}

void ItemObject::deleteItem()
{
	this->occupiedTile->setItem(false);
	this->occupiedTile->setItemID(-1);
	this->occupiedTile->setSpawn(true);

}

