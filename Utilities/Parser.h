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

		void addEventMovement();

		void addEventSpawn();

		void addEventDespawn();

		void addEventAttack();
		void addEventHit();
		void addEventPickUp();

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
