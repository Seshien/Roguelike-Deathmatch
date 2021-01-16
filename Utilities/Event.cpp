#include "Event.h"
using namespace Parser;

Type Parser::convertToType(int type)
{
	if (type >= static_cast<int>(Type::TYPEERROR) && type <= static_cast<int>(Type::ERRORNET))
		return static_cast<Type>(type);
	return Type::TYPEERROR;
}

SubType Parser::convertToSubType(int type)
{
	if (type >= static_cast<int>(SubType::SUBERROR) && type <= static_cast<int>(SubType::ERRORNET))
		return static_cast<SubType>(type);
	return SubType::SUBERROR;
}

std::string Parser::convertToString(Type type)
{
	if (!((int)type >= static_cast<int>(Type::TYPEERROR) && (int)type <= static_cast<int>(Type::ERRORNET)))
		return std::string("Wrong Type");
	else
		return typeNames[(int)type - (int)Type::TYPEERROR];
}

std::string Parser::convertToString(SubType type)
{
	if (!((int)type >= static_cast<int>(SubType::SUBERROR) && (int)type <= static_cast<int>(SubType::ERRORNET)))
		return std::string("Wrong SubType");
	else
		return subTypeNames[(int)type - (int)SubType::SUBERROR];
}

Event Parser::decodeBytes(std::string data)
{
	//sender, receiver, type, subtype, subdata
	Event ev(data[0] - '0', data[1] - '0', (char) data[2], (char) data[3], std::string(data, 4));
	return ev;
}

std::string Parser::encodeBytes(Event ev)
{
	//char data[50];
	std::string data = std::string(ev.size);
	data.push_back(ev.sender + '0');
	data.push_back(ev.receiver + '0');
	data.push_back((char) ev.type);
	data.push_back((char) ev.subtype);
	data.append(ev.subdata);
	return data;
}