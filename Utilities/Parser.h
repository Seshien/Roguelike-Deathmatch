#pragma once
#include <vector>
#include <cstring>
#include <string>
#include "Event.h"
#include "Constants.h"
#include "Logger.h"
namespace Parser
{


	class Messenger
	{
	public:

		Messenger()
		{

		}

		void addEvent(int sender, int receiver, int type, int subtype, std::string);

		void addEvent(int sender, int receiver, Parser::Type type, Parser::SubType subtype, std::string);

		void addEvent(Event ev);

		void addInnerInitPlayer(int sender, int receiver, int ID);

		void addInnerDiscPlayer(int sender, int receiver);

		void addEventInitPlayer(int sender, int receiver, int ID);

		void addEventNewPlayer(int sender, int receiver, int ID);

		void addEventNewPlayer(int sender, int receiver, std::string name);

		void addEventDiscPlayer(int sender, int receiver, std::string name = std::string());

		void addEventTimeoutReached(int sender, int receiver);

		void addEventTimeoutAnswer(int sender, int receiver);
		
		//game events

		void addEventMovement(int sender, int receiver, std::string playerName, int x, int y);
		void addEventMovedOut(int sender, int receiver, std::string playerName, int x, int y);
		void addEventSpawn(int sender, int receiver, int objectType, int x, int y);
		void addEventPlayerSpawn(int sender, int receiver, std::string, int x, int y);
		void addEventKillCount(int sender, int receiver, std::string playerName, int killcount);

		void addEventAskRespawn(int sender, int receiver, int x, int y);
		void addEventWillToRespawn(int sender, int receiver);
		void addEventRespawn(int sender, int receiver, int x, int y);

		void addEventDespawn(int sender, int receiver, int objectType, int x, int y);

		void addEventDespawnPlayer(int sender, int receiver, std::string playerName, int x, int y);

		void addEventAttack(int sender, int receiver, std::string playerName);
		void addEventHit(int sender, int receiver, std::string playerName, int damage);
		void addEventDamaged(int sender, int receiver, int newHealth);

		void addEventPickUp(int sender, int receiver, int itemType);
		
		// client events
		void addEventKeyInput(int sender, int receiver, char key);

		//trzy fazy gry, info dump

		void addEventLobby(int sender, int receiver, int numOfVotes);

		void addEventMidGame(int sender, int receiver, std::chrono::duration<double>);

		void addEventGameEnd(int sender, int receiver, std::string results);

		//id mapy

		void addEventMapID(int sender, int receiver, int mapID);

		//client glosuje, serwer wysyla wiadomosc z liczba glosow

		void addEventVote(int sender, int receiver);
		void addEventVote(int sender, int receiver, int numOfVotes);

		//rozne bledy

		void addEventNoContact(int sender, int receiver);
		void addEventLostConnection(int sender, int receiver);

		void addEventNoAccept(int sender, int receiver);

		std::vector<Event> eventList;

	};

	/*
	//Lobby
	Klient do³¹cza
	Klient wysyla wiadomosc o swoim nicku i checi dolaczenia do gry
	Serwer przyjmuje wiadomosc, sprawdza czy taki gracz istnieje (jezeli tak to odmawia polaczenia), sprawdza czy taki gracz istnial ( jezeli tak to uzywa tego gracza),
	jezeli taki gracz nie istnial, tworzy nowego. W kazdym przypadku wysyla wiadomosc powrotn¹.


	Serwer wysyla mu wiadomosc o ustawieniach lobby
	Serwer wysyla mu wiadomosci o innych graczach
	Serwer wysyla mu wiadomosc o tym ze ktos zaglosowal
	Klient wyslal wiadomosc ze ktos zaglosowal
	Serwer wysyla wiadomosc o poczatku gry
	/Gra
	Serwer wysyla wiadomosc o poczatkowym stanie gracza
	Serwer wysyla wiadomos o

	*/
}
