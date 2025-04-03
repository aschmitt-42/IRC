#include "irc.hpp"

int create_server_socket(void) {

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
    sockaddr.sin_port = htons(4242); // actuellement 4242 mais a changer avec le 1er paramettre
    
    if (bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != 0) {
        std::cerr << "[Server] Bind error: " << strerror(errno) << std::endl;
        return (-1);
    }
    std::cout << "[Server] Bound socket to localhost port " << 4242 << std::endl;

    if (listen(socket_fd, 10) != 0){
        std::cerr << "[Server] Listen error: " << strerror(errno) << std::endl;
        return (-1);
    }
    return socket_fd;
}


void accept_new_connection(int server_socket, struct pollfd **poll_fds, int *poll_count, int *poll_size)
{
    int client_fd;
    char msg_to_send[BUFSIZ];
    int status;

    client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return ;
    }

    add_to_poll_fds(poll_fds, client_fd, poll_count, poll_size);

    printf("[Server] Accepted new connection on client socket %d.\n", client_fd);

    memset(&msg_to_send, '\0', sizeof msg_to_send);
    sprintf(msg_to_send, "Welcome. You are client fd [%d]\n", client_fd);
    status = send(client_fd, msg_to_send, strlen(msg_to_send), 0);
    if (status == -1) {
        fprintf(stderr, "[Server] Send error to client %d: %s\n", client_fd, strerror(errno));
    }
}


void read_data_from_socket(int i, struct pollfd **poll_fds, int *poll_count, int server_socket)
{
    char buffer[BUFSIZ];
    char msg_to_send[BUFSIZ];
    int bytes_read;
    int status;
    int dest_fd;
    int sender_fd;

    sender_fd = (*poll_fds)[i].fd;
    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("[%d] Client socket closed connection.\n", sender_fd);
        }
        else {
            fprintf(stderr, "[Server] Recv error: %s\n", strerror(errno));
        }
        close(sender_fd);
        del_from_poll_fds(poll_fds, i, poll_count);
    }
    else {
        // Renvoie le message reçu à toutes les sockets connectées
        // à part celle du serveur et celle qui l'a envoyée
        printf("[%d] Got message: %s", sender_fd, buffer);
    
        memset(&msg_to_send, '\0', sizeof msg_to_send);
        snprintf(msg_to_send, sizeof(msg_to_send), "[%d] says: %s", sender_fd, buffer);
        // sprintf(msg_to_send, "[%d] says: %s", socket, buffer);
        for (int j = 0; j <= *poll_count; j++) {
            dest_fd = (*poll_fds)[j].fd;
            if (dest_fd != server_socket && dest_fd != sender_fd) {
                status = send(dest_fd, msg_to_send, strlen(msg_to_send), 0);
                if (status == -1) {
                    fprintf(stderr, "[Server] Send error to client fd %d: %s\n", dest_fd, strerror(errno));
                }
            }
        }
    }
}

int    server(void)
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



/*

if (poll(_pfds.begin().base(), _pfds.size(), -1) < 0){ 
            throw std::runtime_error("Error while polling from fd!");
            std::cout << "waiiiiittt" << std::endl;
        }
    

    . Objectif de poll
        La fonction poll permet de surveiller simultanément plusieurs descripteurs de fichiers pour des événements spécifiques. 
        Elle est particulièrement utile dans les applications réseau, comme les serveurs, où plusieurs clients peuvent se connecter en même temps. 
        Cette approche évite l'utilisation de boucles d'attente active, ce qui améliore l'efficacité du programme.


    2. Paramètres passés à poll
        - _pfds.begin().base() : Cette expression retourne un pointeur vers le tableau sous-jacent contenant les structures pollfd dans le conteneur _pfds. 
        Chaque structure pollfd représente un descripteur de fichier surveillé et les événements qui l'intéressent (par exemple, POLLIN pour les données entrantes).
        - _pfds.size() : Ce paramètre indique le nombre de descripteurs de fichiers surveillés, correspondant à la taille du conteneur _pfds.
        - -1 : Ce paramètre représente le délai d'attente (timeout). 
        Une valeur de -1 signifie que la fonction poll bloquera indéfiniment jusqu'à ce qu'au moins un descripteur de fichier soit prêt pour un événement.
*/