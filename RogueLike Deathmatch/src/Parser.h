#pragma once
#include <vector>
#include <cstring>
#include <string.h>
#include "Constants.h"
namespace Parser
{
	enum Type { SLASH = '/', SERVER, LOBBY, GAME};
	enum SubType { ERRSUB = '/', NEWPLAYER, DISCPLAYER, VOTE, TIMEOUT, TIMEOUTANSWER, MOVE, ACTION, CHANGESTATE, START};

	Type convertToType(int type);
	SubType convertToSubType(int type);

	struct Event
	{
		int sender; // od kogo jest ta wiadomosc
		int receiver; // dla kogo jest ta wiadomosc, 0 to serwer, reszta to playerId poszczegolnych graczy
		//serwer, lobby, gra
		Type type;
		//gracz sie polaczyl, gracz sie rozlaczyl, gracz wykonal ruch, gracz wykonal akcje, zmiana stanu gracza, gracz zaglosowal, timeout, 
		SubType subtype; // typ eventu, moze dac jako enumerator 
		char subdata[50];
		Event(int _sender, int _receiver, Type _type, SubType _subtype, char _subdata[])
		{
			sender = _sender;
			receiver = _receiver;
			type = _type;
			subtype = _subtype;
			strncpy_s(subdata, 46, _subdata, 45);
		}
		Event(int _sender, int _receiver, int _type, int _subtype, char _subdata[])
		{
			sender = _sender;
			receiver = _receiver;
			type = convertToType(_type);
			subtype = convertToSubType(_subtype);
			strncpy_s(subdata, 46, _subdata, 45);
		}
	};

	Event decodeBytes(char* data);

	char* encodeBytes(Event ev, char(&data)[50]);

	class Messenger
	{
	public:

		Messenger()
		{

		}

		void addEvent(int sender, int receiver, int type, int subtype, char data[]);

		void addEvent(Event ev);

		void addEventNewPlayer(int sender, int receiver);

		void addEventDiscPlayer(int sender, int receiver);

		void addEventTimeoutReached(int sender, int receiver);

		std::vector<Event> eventList;

	};

	/*
	//Lobby
	Klient do³¹cza
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
