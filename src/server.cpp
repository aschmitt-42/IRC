#include "Server.hpp"
#include "Client.hpp"
#include "irc.hpp"


Server::Server(std::string port, std::string password)
{
	_port = port;
	_password = password;
    _server_socket = create_server_socket(std::atoi(_port.c_str()));
    if (_server_socket == -1) {
        std::cerr << "Error creating server socket" << std::endl;
        exit(1);
    }
    std::cout << "[Server] Listening on port " << _port << std::endl;
}

Server::~Server() 
{ 
    for (size_t i = 0; i < _clients.size(); i++)
    {
        delete _clients[i];
    }
    
    std::cout << "Destructor Server called" << std::endl; 
}


void Server::disconect_client(int client_fd)
{
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
        if (it->fd == client_fd) {
            _poll_fds.erase(it);
            break;
        }
    }
    for (std::size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i]->_client_poll.fd == client_fd)
        {
            _clients.erase(_clients.begin() + i);
            close(client_fd);
            break;
        }
    }

}


void Server::accept_new_connection()
{

    int client_fd = accept(_server_socket, NULL, NULL);
    if (client_fd == -1) {
        std::cerr << "[Server] Accept error: " << strerror(errno) << std::endl;
        return ;
    }

    pollfd tt = {client_fd, POLLIN, 0};
    Client *client = new Client(tt);

    client->_username = intToString(client_fd);
    _poll_fds.push_back(tt);
    _clients.push_back(client);

    std::cout << "[Server] New client connected on fd " << client_fd << std::endl;

    std::string msg = "Welcome. You are client fd [" + intToString(client_fd) + "]\n";
    int status = send(client_fd, msg.c_str(), msg.size(), 0);
    if (status == -1)
        std::cerr << "[Server] Send error to client " << client_fd << ": " << strerror(errno) << std::endl;
}


void Server::read_data_from_socket(int client_fd)
{
    int size = 10;
    char buffer[size];
    std::string msg;
    int status;
    
    memset(buffer, 0, size);
    while (strchr(buffer, '\n') == NULL)
    {
        memset(buffer, 0, size);
        
        status = recv(client_fd, buffer, size, 0);
        if (status < 0) { // && errno != EWOULDBLOCK
            std::cerr << "[Server] Recv error: " << strerror(errno) << std::endl;
            this->disconect_client(client_fd);
            close(client_fd);
            return ;
        }
        else if (status == 0)
        {
            std::cout << "[Server] Client fd " << client_fd << " disconnected" << std::endl;
            this->disconect_client(client_fd);
            close(client_fd);
            return ;
        }
        buffer[size] = 0;
        msg.append(buffer);
    }
    //std::cout << "[" << client_fd << "] Got Message: " << msg << std::endl;
    
    int dest_fd;
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
    {
        dest_fd = it->fd;
        if (dest_fd != _server_socket && dest_fd != client_fd) 
        {
            status = send(dest_fd, msg.c_str(), msg.size(), 0);
            
            if (status == -1) 
                std::cerr << "[Server] Send error to client fd " << dest_fd << ": " << strerror(errno) << std::endl;
        }
    }
    IRC_Parser(msg, this, FINDING_Client(client_fd));
}

Client* Server::FINDING_Client(int client_fd)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->GET_Pollfd().fd == client_fd)
            return _clients[i];
    }
    return NULL;
}


void	Server::start()
{
	std::cout << "---- SERVER ----\n" << std::endl;

    pollfd server_fd = {_server_socket, POLLIN, 0};
    _poll_fds.push_back(server_fd);
    
    while (1) 
    {    
        int status = poll(_poll_fds.begin().base(), _poll_fds.size(), -1);

        if (status == -1) {
            std::cerr << "[Server] Poll error: " << strerror(errno) << std::endl;
            exit(1);
        }
        
        for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) 
        {
            if (it->revents == 0){
                continue;
            }
            
            // if ((it->revents & POLLHUP) == POLLHUP)
            // {
            //     this->disconect_client(it->fd);
            //     std::cout << "POLLHUPPPP" << std::endl;
            //     break;
            // }
            
            if ((it->revents & POLLIN) == POLLIN)
            {
                if (it->fd == _server_socket)
                {
                    this->accept_new_connection();
                    break;
                }
                
                this->read_data_from_socket(it->fd);
                break;
            }
        }
    }
}

// if (it->revents & POLLERR) {
//     std::cerr << "[Server] Error on fd: " << it->fd << std::endl;
//     close(it->fd);
//     (&_poll_fds, it - _poll_fds.begin(), &_poll_fds.size());
//     continue;
// }


//COMMAND

void Server::JOIN()
{
    std::cout << "JOIN DETECTED" << std::endl;
}
void Server::KICK()
{
    std::cout << "KICK DETECTED" << std::endl;
}
void Server::INVITE()
{
    std::cout << "INVITE DETECTED" << std::endl;
}
void Server::TOPIC()
{
    std::cout << "TOPIC DETECTED" << std::endl;
}
void Server::MODE()
{
    std::cout << "MODE DETECTED" << std::endl;
}