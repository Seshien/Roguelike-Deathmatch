#include "Parser.h"
using namespace Parser;
Type Parser::convertToType(int type)
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

SubType Parser::convertToSubType(int type)
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
	return SubType::ERRSUB;
}

Event Parser::decodeBytes(char * data)
{
	//sender, receiver, type, subtype, data
	Event ev(data[0] - 48, data[1] - 48, data[2] - 48, data[3] - 48, data + 4);
	return ev;
}

char * Parser::encodeBytes(Event ev, char(&data)[50])
{
	//char data[50];
	data[0] = ev.sender + 48;
	data[1] = ev.receiver + 48;
	data[2] = ev.type + 48;
	data[3] = ev.subtype + 48;
	strncpy_s(data + 4, 46, ev.subdata, 45);
	return data;
}

void Messenger::addEvent(int sender, int receiver, int type, int subtype, char data[])
{
	Event ev = Event(sender, receiver, type, subtype, data);
	eventList.push_back(ev);
}

void Messenger::addEvent(Event ev) {
	eventList.push_back(ev);
}

void Messenger::addEventNewPlayer(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::NEWPLAYER, NULL);
	eventList.push_back(ev);
}

void Messenger::addEventDiscPlayer(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::DISCPLAYER, NULL);
	eventList.push_back(ev);
}

void Messenger::addEventTimeoutReached(int sender, int receiver) {
	Event ev = Event(sender, receiver, Type::SERVER, SubType::TIMEOUT, NULL);
	eventList.push_back(ev);
}
