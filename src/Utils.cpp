
#include "Server.hpp"

int create_server_socket(int port) {

    // Création de la socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = TCP
    if (socket_fd == -1) {
        std::cerr << "[Server] Socket error: " << strerror(errno) << std::endl;
        return (-1);
    }

    int tmp = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp))){
        std::cerr << "[Server] Socket option error: " << strerror(errno) << std::endl;
        return (-1);
    }

    // fcntl maybe for NON-BLOCKING SERVER

    // Liaison de la socket à l'adresse et au port
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET; // IPv4
    sockaddr.sin_addr.s_addr = INADDR_ANY; // ou pour accessible que localement htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
    sockaddr.sin_port = htons(port); 
    
    if (bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != 0) {
        std::cerr << "[Server] Bind error: " << strerror(errno) << std::endl;
        return (-1);
    }

    if (listen(socket_fd, 10) != 0){
        std::cerr << "[Server] Listen error: " << strerror(errno) << std::endl;
        return (-1);
    }
    return socket_fd;
}

std::string intToString(int number) 
{
    std::ostringstream oss;
    oss << number;
    return oss.str();
}

void	ERR(Client *client, int err_id, std::string command, std::string msg)
{
    std::cout << "ERR DETECTED ID:"  << err_id << std::endl;
    
    std::string err_msg;
    
	if (!client)
        return;
    
	if (command.empty())
        err_msg = ":localhost " + intToString(err_id) + " :" + msg;
	else
        err_msg = ":localhost " + intToString(err_id) + " " + command + " :" + msg;

	client->Send_message(err_msg);
}