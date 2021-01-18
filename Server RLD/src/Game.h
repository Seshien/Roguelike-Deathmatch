#pragma once
#include "SpawnableObject.h"
#include "PlayerObject.h"
#include "Map.h"
#include "..\..\Utilities\Event.h"
#include "..\..\Utilities\Logger.h"
#include "..\..\Utilities\Parser.h"
#include "..\..\Utilities\Constants.h"

class Game
{
public:
	Game()
	{

	}

	void startGame()
	{

	}

	void startMap(std::string mapPath)
	{
		this->map.init(mapPath);

	}

	Parser::Messenger loopObjectEvents(Parser::Messenger input)
	{
		output = Parser::Messenger();
		for (auto object : gameObjectList)
		{
			//something
		}
		for (auto object : playerObjectList)
		{
			//something
		}
	}

	void addPlayer(int playerID)
	{
		// znalezc jakies miejsce gdzie moze sie zespawnic
		playerObjectList.push_back(std::make_shared<PlayerObject>(playerID, SpawnableObjectType::PLAYER, playerID + 2, playerID + 2));

	}

	void killPlayer(PlayerObject * player)
	{
		player->setAlive(false);
		// tworzyc nowy obiekt z trupem
		// 
	}

	void deletePlayer(int playerID)
	{
		int index = findPlayerIndex(playerID);
		if (index == -1)
		{
			Logger::log("Error: Delete Player failure, player not found");
			return;
		}

		killPlayer(playerObjectList[index].get());

		playerObjectList.erase(playerObjectList.begin() + index);

	}

	int findPlayerIndex(int playerID)
	{
		for (int i = 0; i < playerObjectList.size(); i++)
			if (playerObjectList[i]->getplayerID() == playerID)
				return i;

		return -1;

	}

private:

	Map map;
	Parser::Messenger output;
	std::vector<std::shared_ptr<SpawnableObject>> gameObjectList;
	std::vector<std::shared_ptr<PlayerObject>> playerObjectList;
};

