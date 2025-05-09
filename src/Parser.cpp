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
		std::string possible_cmd[9] = {"PASS", "NICK", "USER", "QUIT", "KICK", "JOIN", "TOPIC", "INVITE", "MODE"};
		for (size_t i = 0; i != 9; i++)
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
	if (client->get_channel())
		client->get_channel()->SEND_Msg(msg, client);
	(void)serv;
}

void IRC_Parser(std::string msg, Server *serv, Client *client)
{
	std::string cmd = CMD_Finder(msg);
	//std::cout << "MSG : " << msg << "CMD : " << cmd << std::endl;
	if (cmd.empty())
	{
		std::cout << "cmd empty\n" <<std::endl;
		PRINT_Msg(msg, serv, client);
		return;
	}

	std::string	argument = ARG_Finder(msg);

	if (cmd == "PASS")
		serv->PASS(client, argument);
	// else if (cmd == "NICK")
	// 	serv->NICK(client, argument);
	// else if (cmd == "USER")
	// 	serv->USER(client, argument);
	// else if (cmd == "QUIT")
	// 	serv->QUIT(client, argument);
	
	
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
}