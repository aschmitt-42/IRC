#include "Client.hpp"


Client::Client(pollfd client_poll, int client_fd)
{
	_client_fd = client_fd;
	
	_client_poll = client_poll;

	_nickname = "";
	_msg = "";
	_username = "";
	_hostname = "";
	
	_registred_password = 0;
	_registred_user = 0;
}

Client::~Client()
{
	// std::cout << "Destructor called on " << _nickname << std::endl;
}

void Client::SEND_Quit_Msg(std::string msg)
{
	std::vector<Client*> sended_client;
    std::vector<Client*> tmp;

	for (size_t i = 0; i < _channels.size(); ++i)
	{
		tmp = _channels[i]->GET_Clients_Vector();

		for (std::vector<Client*>::iterator it = tmp.begin(); it != tmp.end(); ++it)
		{
			if (!IS_Client_In_Vector(sended_client, *it) && this != *it)
			{
				(*it)->Send_message(":" + _nickname + "!" + _username + "@localhost" + " QUIT :" + msg);
				sended_client.push_back(*it);
			}
		}
	}

	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		(*it)->DELETE_User(this);
	}
}

void Client::Send_message(std::string msg)
{
	msg += "\r\n";

	if (send(_client_fd, msg.c_str(), msg.size(), 0) == -1)
	{
		std::cerr << "Error sending message to client" << std::endl;
	}
}

int	Client::REGISTRED()
{
	if (!_nickname.empty() && _registred_user == 1 && _registred_password == 1)
		return 1;
	return 0;
}

void	Client::SET_Username(std::vector<std::string> argument, std::string msg)
{
	_username = argument[0];
	_mode = std::atoi(argument[1].c_str());
	_hostname = "localhost";
	_realname = msg;
}

void	Client::Join_Channel(Channel *channel)
{
	_channels.push_back(channel);
}

std::string	Client::get_Prefix() const
{
	return (_nickname + "!" + _username + "@" + _hostname);
}

void Client::Leave_Channel(Channel *channel)
{
	for (size_t i = 0; i < _channels.size(); ++i)
	{
		if (_channels[i]->GET_Name() == channel->GET_Name())
		{
			_channels.erase(_channels.begin() + i);
			break;
		}
	}
}