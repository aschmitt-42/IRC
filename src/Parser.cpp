#include "Server.hpp"
#include "Client.hpp"
#include "irc.hpp"

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
		std::string possible_cmd[5] = {"KICK", "JOIN", "TOPIC", "INVITE", "MODE"};
		for (size_t i = 0; i != 5; i++)
		{
			if (cmd == possible_cmd[i])
				return cmd;
		}
		cmd.clear();
	}
	return cmd;
}

std::string ARG_Finder(std::string msg)
{
    std::istringstream stream(msg);
    std::string word;

    stream >> word;

    if (stream >> word)
        return word;
	else 
        return "";
}

void PRINT_Msg(std::string msg, Server *serv, Client *client)
{
	if (client->GET_Channel())
		client->GET_Channel()->SEND_Msg(msg, client);
	(void)serv;
}

void IRC_Parser(std::string msg, Server *serv, Client *client)
{
	if (client == NULL)
	{
		std::cout << "client non trouver" << std::endl;
		return;
	}

	std::string cmd = CMD_Finder(msg);
	//std::cout << "MSG : " << msg << "CMD : " << cmd << std::endl;
	if (cmd.empty())
	{
		std::cout << "cmd empty\n" <<std::endl;
		PRINT_Msg(msg, serv, client);
		return;
	}

	std::string	argument = ARG_Finder(msg);
	if (cmd == "JOIN")
		serv->JOIN(client, argument);
	else if (cmd == "KICK")
		serv->KICK(client, argument);
	else if (cmd == "INVITE")
		serv->INVITE(client, argument);
	else if (cmd == "TOPIC")
		serv->TOPIC(client, argument);
	else if (cmd == "MODE")
		serv->MODE(client, argument);
	else
		std::cout << "WTFFF" << std::endl;
}