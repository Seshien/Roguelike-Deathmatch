#pragma once
#include <vector>
#include <cstring>
#include <string>
#include "Constants.h"
namespace Parser
{
	enum Type { SLASH = '/', SERVER, LOBBY, GAME};
	enum SubType { ERRSUB = '/', NEWPLAYER, DISCPLAYER, VOTE, TIMEOUT, TIMEOUTANSWER, MOVE, ACTION, CHANGESTATE, START};

	Type convertToType(int type);
	SubType convertToSubType(int type);

	struct Event
	{
		std::string size;
		int sender; // od kogo jest ta wiadomosc
		int receiver; // dla kogo jest ta wiadomosc, 0 to serwer, reszta to playerId poszczegolnych graczy
		//serwer, lobby, gra
		Type type;
		//gracz sie polaczyl, gracz sie rozlaczyl, gracz wykonal ruch, gracz wykonal akcje, zmiana stanu gracza, gracz zaglosowal, timeout, 
		SubType subtype; // typ eventu, moze dac jako enumerator 
		std::string subdata;

		Event(int _sender, int _receiver, Type _type, SubType _subtype, std::string _subdata)
		{
			size = std::to_string((4 * sizeof(char)+ sizeof(_subdata))/8) + "|";
			sender = _sender;
			receiver = _receiver;
			type = _type;
			subtype = _subtype;
			subdata = std::string(_subdata);
		}
		Event(int _sender, int _receiver, int _type, int _subtype, std::string _subdata)
		{
			size = std::to_string((4 * sizeof(char) + sizeof(_subdata)) / 8) + "|";
			sender = _sender;
			receiver = _receiver;
			type = convertToType(_type);
			subtype = convertToSubType(_subtype);
			subdata = std::string(_subdata);
		}
	};

	Event decodeBytes(std::string data);

	std::string encodeBytes(Event ev);

	class Messenger
	{
	public:

		Messenger()
		{

		}

		void addEvent(int sender, int receiver, int type, int subtype, std::string);

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
