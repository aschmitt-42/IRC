#include "Client.hpp"


Client::Client(pollfd client_poll) : _client_poll(client_poll)
{
	
}

Client::~Client()
{
	// std::cout << "Destructor called" << std::endl;
}

