#include "Client.hpp"


Client::Client(pollfd client_poll, int client_fd)
{
	_operator = 0;
	_client_fd = client_fd;
	
	_client_poll = client_poll;
	_channel = NULL;

	_nickname = "";
	_msg = "";
	_username = "";
	
	_registred_password = 0;
	_registred_user = 0;
}

Client::~Client()
{
	// std::cout << "Destructor called" << std::endl;
}

void Client::Send_message(std::string msg)
{
	msg += "\r\n";
	if (send(_client_fd, msg.c_str(), msg.size(), 0) == -1)
	{
		std::cerr << "Error sending message to client" << std::endl;
	}
}

