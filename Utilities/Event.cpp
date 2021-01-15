#include "Event.h"
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
	if (type >= ERRSUB && type <= ERRORNET)
		return static_cast<SubType>(type);
	return SubType::ERRSUB;
}

Event Parser::decodeBytes(std::string data)
{
	//sender, receiver, type, subtype, data
	Event ev(data[0] - '0', data[1] - '0', (Type) data[2],(SubType) data[3], std::string(data, 4));
	return ev;
}

std::string Parser::encodeBytes(Event ev)
{
	//char data[50];
	std::string data = std::string(ev.size);
	data.push_back(ev.sender + '0');
	data.push_back(ev.receiver + '0');
	data.push_back(ev.type);
	data.push_back(ev.subtype);
	data.append(ev.subdata);
	return data;
}