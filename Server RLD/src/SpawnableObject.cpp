#include "SpawnableObject.h"



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

