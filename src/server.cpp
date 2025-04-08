#include "Server.hpp"

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

Server::Server(const Server& other)
{
	(void)other;
	// std::cout << "Recopy constructor called" << std::endl;
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
    char msg_to_send[BUFSIZ];
    std::cout << "Startt accept_new_connection" << std::endl;
    int client_fd = accept(_server_socket, NULL, NULL);
    if (client_fd == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return ;
    }

    pollfd clien_poll = {client_fd, POLLIN, 0};
    _poll_fds.push_back(clien_poll);

    printf("[Server] Accepted new connection on client socket %d.\n", client_fd);

    memset(&msg_to_send, '\0', sizeof msg_to_send);
    sprintf(msg_to_send, "Welcome. You are client fd [%d]\n", client_fd);
    int status = send(client_fd, msg_to_send, strlen(msg_to_send), 0);
    if (status == -1) {
        fprintf(stderr, "[Server] Send error to client %d: %s\n", client_fd, strerror(errno));
    }
    std::cout << "ENDDD accept_new_connection" << std::endl;
}


void Server::read_data_from_socket(int client_fd)
{
    char buffer[BUFSIZ];
    char msg_to_send[BUFSIZ];
    int bytes_read;
    int status;
    int dest_fd;
    std::cout << "Starttt read_data_from_socket" << std::endl;

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(client_fd, buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("[%d] Client socket closed connection.\n", client_fd);
        }
        else {
            fprintf(stderr, "[Server] Recv error: %s\n", strerror(errno));
        }
        this->del_from_poll_fds(client_fd);
        close(client_fd);
    }
    else {
        // Renvoie le message reçu à toutes les sockets connectées
        // à part celle du serveur et celle qui l'a envoyée
        printf("[%d] Got message: %s", client_fd, buffer);
    
        memset(&msg_to_send, '\0', sizeof msg_to_send);
        snprintf(msg_to_send, sizeof(msg_to_send), "[%d] says: %s", client_fd, buffer);
        // sprintf(msg_to_send, "[%d] says: %s", socket, buffer);

        for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it){
            dest_fd = it->fd;
            if (dest_fd != _server_socket && dest_fd != client_fd) {
                status = send(dest_fd, msg_to_send, strlen(msg_to_send), 0);
                if (status == -1) {
                    fprintf(stderr, "[Server] Send error to client fd %d: %s\n", dest_fd, strerror(errno));
                }
            }
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

        int status = poll(_poll_fds.begin().base(), _poll_fds.size(), 2000);
        if (status == -1) {
            std::cerr << "[Server] Poll error: " << strerror(errno) << std::endl;
            exit(1);
        }
        else if (status == 0) {
            std::cout << "[Server] Waiting...\n";
            continue;
        }
        for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
            if (it.base()->revents == 0){
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
                    break;
                }
                
                this->read_data_from_socket(it->fd);
            }

            // if (it->revents & POLLERR) {
            //     std::cerr << "[Server] Error on fd: " << it->fd << std::endl;
            //     close(it->fd);
            //     del_from_poll_fds(&_poll_fds, it - _poll_fds.begin(), &_poll_fds.size());
            //     continue;
            // }
            // if (it->revents & POLLHUP) { // ((it->revents & POLLHUP) == POLLHUP)
            //     std::cerr << "[Server] Hangup on fd: " << it->fd << std::endl;
            //     close(it->fd);
            //     del_from_poll_fds(&_poll_fds, it - _poll_fds.begin(), &_poll_fds.size());
            //     continue;
            // }
        }
            // if ((poll_fds[i].revents & POLLIN) != 1) {
            //     printf("caca i = %d et revevents: %d\n", i, poll_fds[i].revents);
            //     continue ;
            // }
    }
}

