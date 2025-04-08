#include "Server.hpp"

Server::Server(std::string port, std::string password)
{
	_port = port;
	password = password;
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

Server& Server::operator=(const Server &other)
{
	(void)other;
	return *this;
	// std::cout << "Assignment operator called" << std::endl;
}

void	Server::start()
{
	std::cout << "---- SERVER ----\n" << std::endl;

    struct pollfd *poll_fds; // Tableau de descripteurs
    int poll_size; // Taille du tableau
    int poll_count; // Nombre actuel de descripteurs

    int server_socket = create_server_socket();
    if (server_socket == -1) {
        return (1);
    }
    std::cout << "[Server] Listening on port " << 4242 << std::endl;


    // Préparation du tableau des descripteurs de fichier pour poll()
    poll_size = 10;
    poll_fds = (pollfd*)calloc(poll_size + 1, sizeof *poll_fds);
    if (!poll_fds) {
        return (4);
    }

    poll_fds[0].fd = server_socket;
    poll_fds[0].events = POLLIN;
    poll_count = 1;

    printf("[Server] Set up poll fd array\n");

    while (1) {

        // Sonde les sockets prêtes (avec timeout de 2 secondes)
        int status = poll(poll_fds, poll_count, 2000);
        if (status == -1) {
            fprintf(stderr, "[Server] Poll error: %s\n", strerror(errno));
            exit(1);
        }
        else if (status == 0) {
            printf("[Server] Waiting...\n");
            continue;
        }

        for (int i = 0; i < poll_count; i++) {
            printf("i = %d et revevents: %d\n", i, poll_fds[i].revents);
            if ((poll_fds[i].revents & POLLIN) != 1) {
                printf("caca i = %d et revevents: %d\n", i, poll_fds[i].revents);
                continue ;
            }
            printf("[%d] Ready for I/O operation\n", poll_fds[i].fd);
            if (poll_fds[i].fd == server_socket) {
                accept_new_connection(server_socket, &poll_fds, &poll_count, &poll_size);
            }
            else {
                read_data_from_socket(i, &poll_fds, &poll_count, server_socket);
            }
        }
    }

    
    return (0);
}
