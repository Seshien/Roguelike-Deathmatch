#include "Server.h"

void Server::startLogger()
{
	Logger::startLogger();
}

// void GenerateMap();

void Server::startMap()
{
	this->map.init(mapPath);
}

void Server::loopLobby()
{
	while (true)
	{
		this->input = this->network.inputNetwork();
		handleEvents(this->input);
		this->network.outputNetwork(this->output);
	}

}

void Server::handleEvents(Parser::Messenger parser)
{
	this->output = Parser::Messenger();
	for (auto& ev : parser.eventList) {
		// Wykonaj logike
		//sender, receiver, type, subdata
		Parser::Event newEv = { 0, ev.sender, ev.type, ev.subtype, ev.subdata };
		// Wpisz wynik w output parser
		this->output.addEvent(newEv);
	}
}

void Server::loadConfig()
{
	std::ifstream file;
	std::string line;
	file.open(this->confName);
	if (file.is_open())
	{
		Logger::log("Config file opened:");
		while (std::getline(file, line))
		{
			Logger::log(line);
			this->processConfigLine(line);
		}
	}

	else Logger::log("Config file not found");
	file.close();
}

void Server::processConfigLine(std::string line)
{
	std::string delimiter = ":";
	int pos = line.find(delimiter);
	if (pos == -1)
	{
		Logger::log("Error during parsing of config file");
		return;
	}
	std::string token = line.substr(0, pos);
	std::string value = line.substr(pos + 1, line.length() - 1);
	Logger::log("Token: " + token + " Value: " + value);
	this->setConfigValue(token, value);
	return;
}

void Server::setConfigValue(std::string token, std::string value)
{
	if (token == "time") this->time = std::stoi(value);
	else if (token == "special") return;
	else if (token == "port") this->port = value;
	else Logger::log("Unknown line in config file");
}
