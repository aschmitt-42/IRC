#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

Channel::Channel(std::string channel_name, std::string topic_name)
{
	_name = channel_name;
	_topic = topic_name;
	std::cout << "Channel " << _name << " successfully created !" << std::endl;
}
Channel::~Channel(){}


void	Channel::ADD_User(Client *client)
{
	_client.push_back(client);
	client->JOIN_Channel(this);
}

void	Channel::DELETE_User(Client *client)
{
	for (size_t	i = 0; i < _client.size(); ++i)
	{
		if (_client[i] == client)
		{
			_client.erase(_client.begin() + i);
			client->DELETE_Channel();
		}
	}
}

void	Channel::SEND_Msg(std::string msg, Client *client)
{
	int status;
	msg = "[" + client->get_username() + "]" + " : " + msg;

	for (size_t	i = 0; i < _client.size(); ++i)
	{
		if (_client[i] != client)
			status = send(_client[i]->get_clientfd(), msg.c_str(), msg.size(), 0);
	}
	(void)status;
}

