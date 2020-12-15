#pragma once
#include <vector>
#include <cstring>
#include <string.h>
#include "Constants.h"
namespace Parser
{
	enum Type { SLASH = '/', SERVER, LOBBY, GAME};
	enum SubType { SLASH = '/', NEWPLAYER, DISCPLAYER, VOTE, TIMEOUT, TIMEOUTANSWER, MOVE, ACTION, CHANGESTATE, START};

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

	Type convertToType(int type)
	{
		switch (type)
		{
			//mozna to tak zrobic
			case static_cast<int>(Type::SERVER) :
				return static_cast<Type>(type);
				break;
			case static_cast<int>(Type::LOBBY) :
				return static_cast<Type>(type);
				break;
			case static_cast<int>(Type::GAME) :
				return static_cast<Type>(type);
				break;
			default:
				return Type::SLASH;
		}
		//albo tak
		if (type >= 47 && type <= 50)
			return static_cast<Type>(type);
	}
	SubType convertToSubType(int type)
	{
		/*
		switch (type)
		{
			//mozna to tak zrobic
			case static_cast<int>(SubType::SERVER):
				return static_cast<SubType>(type);
				break;
			case static_cast<int>(SubType::LOBBY):
				return static_cast<SubType>(type);
				break;
			case static_cast<int>(SubType::GAME):
				return static_cast<SubType>(type);
				break;
			default:
				return SubType::SLASH;
		}
		*/
		//albo tak
		if (type >= 47 && type <= 55)
			return static_cast<SubType>(type);
	}
	Event decodeBytes(char* data)
	{
		//sender, receiver, type, subtype, data
		Event ev(data[0] - 48, data[1] - 48, data[2] - 48, data[3] - 48, data + 4);
		return ev;
	}

	char* encodeBytes(Event ev, char(&data)[50])
	{
		//char data[50];
		data[0] = ev.sender + 48;
		data[1] = ev.receiver + 48;
		data[2] = ev.type + 48;
		data[3] = ev.subtype + 48;
		strncpy_s(data + 4, 46, ev.subdata, 45);
		return data;
	}

	class Messenger
	{
	public:

		Messenger()
		{

		}

		void addEvent(int sender, int receiver, int type, int subtype, char data[])
		{
			Event ev = Event(sender, receiver, type, subtype, data);
			eventList.push_back(ev);
		}

		void addEvent(Event ev) {
			eventList.push_back(ev);
		}

		void addEventNewPlayer(int sender, int receiver) {
			Event ev = Event(sender, receiver, Type::SERVER, SubType::NEWPLAYER, NULL);
			eventList.push_back(ev);
		}

		void addEventDiscPlayer(int sender, int receiver) {
			Event ev = Event(sender, receiver, Type::SERVER, SubType::DISCPLAYER, NULL);
			eventList.push_back(ev);
		}

		void addEventTimeoutReached(int sender, int receiver) {
			Event ev = Event(sender, receiver, Type::SERVER, SubType::TIMEOUT, NULL);
			eventList.push_back(ev);
		}

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
