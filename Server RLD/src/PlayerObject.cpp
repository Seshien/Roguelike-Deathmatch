#include "PlayerObject.h"

PlayerObject::PlayerObject(int id, SpawnableObjectType type, int x, int y) :SpawnableObject(type, x, y){
	this->playerID = id;
};
