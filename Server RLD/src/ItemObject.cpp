#include "ItemObject.h"

ItemObject::ItemObject(int itemID, SpawnableObjectType type, std::shared_ptr<Tile> tile) {
	this->type = type;
	this->isExist = false;
	this->itemID = itemID;
	this->occupiedTile = tile;
	this->occupiedTile->setItem(false);
	this->occupiedTile->setItemID(itemID);
	this->occupiedTile->setSolid(false);
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
	//lepiej to zrobic
	this->type = (SpawnableObjectType) (((int)type + 1) % 4);
	// Operacje na obiekcie map
}

void ItemObject::deleteItem()
{
	this->occupiedTile->setItem(false);
	this->occupiedTile->setItemID(-1);
	this->occupiedTile->setSolid(true);

}

