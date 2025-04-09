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
	std::cout << "Destructor Server called" << std::endl;
}


void Server::del_from_poll_fds(int client_fd) 
{
    std::cout << "Starttt del_from_poll_fds" << std::endl;
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
        if (it->fd == client_fd) {
            close(client_fd);
            _poll_fds.erase(it);
            break;
        }
    }
    std::cout << "ENDDD del_from_poll_fds" << std::endl;
}


void Server::accept_new_connection()
{
    std::cout << "Startt accept_new_connection" << std::endl;

    int client_fd = accept(_server_socket, NULL, NULL);
    if (client_fd == -1) {
        std::cerr << "[Server] Accept error: " << strerror(errno) << std::endl;
        return ;
    }

    pollfd tt = {client_fd, POLLIN, 0};
    Client *client = new Client(tt);
    _poll_fds.push_back(tt);
    _clients.push_back(client);

    std::cout << "[Server] New client connected on fd " << client_fd << std::endl;

    std::string msg = "Welcome. You are client fd [" + intToString(client_fd) + "]\n";
    int status = send(client_fd, msg.c_str(), msg.size(), 0);
    if (status == -1)
        std::cerr << "[Server] Send error to client " << client_fd << ": " << strerror(errno) << std::endl;
        
    std::cout << "ENDDD accept_new_connection" << std::endl;
}


void Server::read_data_from_socket(int client_fd)
{
    char buffer[1024];
    std::string msg_to_send;
    int status;
    int dest_fd;
    std::cout << "Starttt read_data_from_socket" << std::endl;

    memset(buffer, '\0', 1024);
    int i = -1;
    while (strchr(buffer, '\n') == NULL)
    {
        std::cout << "nb tour boucle = " << ++i << std::endl;
        memset(buffer, '\0', 1024);

        status = recv(client_fd, buffer, 1024, 0);
        if (status < 0) { // && errno != EWOULDBLOCK
            std::cerr << "[Server] Recv error: " << strerror(errno) << std::endl;
            this->del_from_poll_fds(client_fd);
            close(client_fd);
            return ;
        }
        if (status == 0)
        {
            std::cout << "[Server] Client fd " << client_fd << " disconnected" << std::endl;
            this->del_from_poll_fds(client_fd);
            close(client_fd);
            return ;
        }
        
        // status == 0 client est parti

        msg_to_send.append(buffer);

    }
    
    std::cout << "[" << client_fd << "] Got Message: " << msg_to_send << std::endl;

    msg_to_send = intToString(client_fd) + " says: " + msg_to_send;

    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
    {
        dest_fd = it->fd;
        if (dest_fd != _server_socket && dest_fd != client_fd) 
        {
            status = send(dest_fd, msg_to_send.c_str(), msg_to_send.size(), 0);
            
            if (status == -1) 
                std::cerr << "[Server] Send error to client fd " << dest_fd << ": " << strerror(errno) << std::endl;
        }
        
    }
    std::cout << "ENDDD read_data_from_socket" << std::endl;
}


void	Server::start()
{
	std::cout << "---- SERVER ----\n" << std::endl;

    pollfd server_fd = {_server_socket, POLLIN, 0};
    _poll_fds.push_back(server_fd);
    
    while (1) {
        
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
            
            if ((it->revents & POLLHUP) == POLLHUP)
            {
                this->del_from_poll_fds(it->fd);
                break;
            }
            
            if ((it->revents & POLLIN) == POLLIN)
            {
                if (it->fd == _server_socket)
                {
                    this->accept_new_connection();
                    for (std::vector<Client*>::iterator it2 = _clients.begin(); it2 != _clients.end(); ++it2)
                    {
                        std::cout << "Client name: " << (*it2)->_username << std::endl;
                    }
                    break;
                }
                
                this->read_data_from_socket(it->fd);
                it->revents = 0;
                break;
            }
        }
    }
}

// if (it->revents & POLLERR) {
//     std::cerr << "[Server] Error on fd: " << it->fd << std::endl;
//     close(it->fd);
//     del_from_poll_fds(&_poll_fds, it - _poll_fds.begin(), &_poll_fds.size());
//     continue;
// }