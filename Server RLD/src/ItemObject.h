#pragma once
#include <iostream>
#include "..\..\Utilities\Constants.h"
#include "SpawnableObject.h"
#include "Tile.h"

class ItemObject : public SpawnableObject
{

private:
	int itemID;

public:

	ItemObject(int itemID, SpawnableObjectType type, std::shared_ptr<Tile> tile);
	int getItemID() { return this->itemID; };

	void spawn();

	void despawn();

	void deleteItem();

};



