#include "Server.hpp"
#include "Client.hpp"

std::string CMD_Finder(std::string msg)
{
	std::string cmd;
	size_t		pos;
	
	pos = msg.find(' ');
	if (pos == std::string::npos)
		pos = msg.find('\n');
	if (pos != std::string::npos)
	{
		cmd = msg.substr(0, pos);
		std::string possible_cmd[13] = {"PASS", "NICK", "USER", "QUIT", "PONG", "PING", "PRIVMSG", "KICK", "JOIN", "TOPIC", "INVITE", "MODE", "PART"};
		for (size_t i = 0; i != 13; i++)
		{
			if (cmd == possible_cmd[i])
				return cmd;
		}
		cmd.clear();
	}
	return cmd;
}

std::vector<std::string> ARG_Finder(std::string msg)
{
    std::vector<std::string> words;
    std::string word;

    
	std::stringstream ss(msg);
    ss >> word;
	while (ss >> word) 
	{
        words.push_back(word);
		// std::cout << word << std::endl;
    }
	return words;
}


void IRC_Parser(std::string msg, Server *serv, Client *client)
{
	std::cout << "\n-------------IRC_PARSER-------------" << std::endl;
	std::cout << msg << std::endl;
	std::cout << "------------------------------------\n" << std::endl;

	std::string cmd = CMD_Finder(msg);
	if (cmd.empty() && client->_registred_user)
	{
		std::cout << "CMD EMPTY" << std::endl;
		// PRINT_Msg(msg, serv, client);
		return;
	}

	std::vector<std::string> argument = ARG_Finder(msg);
	
	
	if (cmd == "PASS")
		return serv->PASS(client, argument);
	else if (cmd == "NICK")
		return serv->NICK(client, argument);
	else if (cmd == "USER")
		return serv->USER(client, argument);
	else if (cmd == "QUIT")
		return serv->QUIT(client, msg);
	else if (cmd == "PING")
		return serv->PING(client, argument);
	
	if (!client->REGISTRED())
	{
		if (client->get_nick().empty()) 
			ERR(client, 451, "*", "You have not registered");
		else
			ERR(client, 451, client->get_nick(), "You have not registered");
		return;
	}
	
	if (cmd == "JOIN")
		serv->JOIN(client, argument);
	else if (cmd == "PRIVMSG")
		return serv->PRIVMSG(client, argument, msg);
	else if (cmd == "INVITE")
		serv->INVITE(client, argument);
	else if (cmd == "MODE")
		serv->MODE(client, argument);
	else if (cmd == "TOPIC")
		serv->TOPIC(client, argument);
	else if (cmd == "KICK")
	 	serv->KICK(client, argument, msg);
	else if (cmd == "PART")
		serv->PART(client, argument, msg);
}