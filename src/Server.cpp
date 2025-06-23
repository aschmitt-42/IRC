#include "Server.hpp"
#include "Client.hpp"

bool running(true);

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
    for (size_t i = 0; i < _disconect_client.size(); i++)
    {
        delete _disconect_client[i];
    }
    for (size_t i = 0; i < _clients.size(); i++)
    {
        delete _clients[i];
    }
    for (size_t i = 0; i < _channels.size(); i++)
    {
        delete _channels[i];
    }
    
    //std::cout << "Destructor Server called" << std::endl; 
}


void Server::disconect_client(Client *client)
{
    std::cout << "Client " << client->get_nick() << " Quitted IRC, Farawell..." << std::endl;
    
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
        if (it->fd == client->get_clientfd()) {
            _poll_fds.erase(it);
            break;
        }
    }

    for (std::size_t i = 0; i < _clients.size(); i++){
        if (_clients[i]->get_nick() == client->get_nick())
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }

    close(client->get_clientfd());

    _disconect_client.push_back(client);
}


void Server::accept_new_connection()
{

    int client_fd = accept(_server_socket, NULL, NULL);
    if (client_fd == -1) {
        std::cerr << "[Server] Accept error: " << strerror(errno) << std::endl;
        return ;
    }

    pollfd tt = {client_fd, POLLIN, 0};
    Client *client = new Client(tt, client_fd);

    //->REGISTRATION();
    _poll_fds.push_back(tt);
    _clients.push_back(client);

    std::cout << "[Server] New client connected on fd " << client_fd << std::endl;

    std::string msg = "new connection\r\n";
    int status = send(client_fd, msg.c_str(), msg.size(), 0);
    if (status == -1)
        std::cerr << "[Server] Send error to client " << client_fd << ": " << strerror(errno) << std::endl;
}


void Server::read_data_from_socket(Client *client)
{
    if (std::find(_clients.begin(), _clients.end(), client) == _clients.end()) 
    {
        std::cerr << "[Server] Attempted to read from a disconnected client" << std::endl;
        return;
    }

    char buffer[1024];
    
    int bytes_read = recv(client->get_clientfd(), buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) { 
        this->disconect_client(client);
        return ;
    }
    buffer[bytes_read] = 0;

    std::string& msg = client->get_message();
    msg.append(buffer);
    
    while (!msg.empty()) 
    {
        size_t position = msg.find('\n');
    
        if (position == std::string::npos) {
            return;
        }
    
        std::string cmd(msg.c_str(), position);
        if (cmd[cmd.size() - 1] == '\r')
            cmd.erase(cmd.size() - 1);
        
        IRC_Parser(cmd, this, client);
        msg.erase(0, position + 1);
	    std::cout << "------------------------------------\n" << std::endl;
    }
}

Client* Server::FINDING_Client_fd(int client_fd)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->get_pollfd().fd == client_fd)
            return _clients[i];
    }
    return NULL;
}


Client* Server::FINDING_Client_str(std::string  username)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->get_nick() == username)
            return _clients[i];
    }
    return NULL;
}

void	Server::start()
{
	std::cout << "---- SERVER ----\n" << std::endl;

    pollfd server_fd = {_server_socket, POLLIN, 0};
    _poll_fds.push_back(server_fd);
    while (running) 
    {    
        int status = poll(_poll_fds.begin().base(), _poll_fds.size(), -1);
        
        if (status == -1) {
            std::cerr << "[Server] Poll error: " << strerror(errno) << std::endl;
            std::cout << "Server Stopper, Bye !!!" << std::endl;
            return;
        }
        
        Client *client;

        for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) 
        {
            if (it->revents == 0){
                continue;
            }
            
            if ((it->revents & POLLIN) == POLLIN)
            {
                if (it->fd == _server_socket)
                {
                    this->accept_new_connection();
                    break;
                }
                client = FINDING_Client_fd(it->fd);
                this->read_data_from_socket(client);
                break;
            }
        }
    }

}

void signal_handler(int signum) 
{
    std::cout << "\n[INFO] SIGNAL : " << signum << " RECEIVED\n";
    running = false;
}


// if (it->revents & POLLERR) {
//     std::cerr << "[Server] Error on fd: " << it->fd << std::endl;
//     close(it->fd);
//     (&_poll_fds, it - _poll_fds.begin(), &_poll_fds.size());
//     continue;
// }



