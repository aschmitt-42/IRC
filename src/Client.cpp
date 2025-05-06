#include "Client.hpp"


Client::Client(pollfd client_poll, int client_fd) : _operator(0),_client_poll(client_poll)
{
	(void)_operator;
	char buffer[1024] = {0};
	int status;
	std::string msg;
	(void)status;

	_client_fd = client_fd;
	msg = "USERNAME : ";
	status = send(client_fd, msg.c_str(), msg.size(), 0);
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read > 0) 
	{
        buffer[bytes_read -1] = '\0';  // null-terminate
        _username = buffer;
        //std::cout << "Utilisateur connecté : " << username << std::endl;
	}   
	else
        std::cerr << "Erreur ou client déconnecté." << std::endl;

	msg = "NICKNAME : ";
	status = send(client_fd, msg.c_str(), msg.size(), 0);
    bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read > 0) 
	{
        buffer[bytes_read - 1] = '\0';  // null-terminate
        _nickname = buffer;
    } 
	else 
        std::cerr << "Erreur ou client déconnecté." << std::endl;
}

Client::~Client()
{
	// std::cout << "Destructor called" << std::endl;
}

