#include "irc.hpp"

int create_server_socket(void) {
    struct sockaddr_in sa;
    int socket_fd;
    int status;

    
    // Préparaton de l'adresse et du port pour la socket de notre serveur
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; // IPv4
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
    sa.sin_port = htons(4242);

    // Création de la socket
    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "[Server] Socket error: %s\n", strerror(errno));
        return (-1);
    }
    printf("[Server] Created server socket fd: %d\n", socket_fd);

    // Liaison de la socket à l'adresse et au port
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0) {
        fprintf(stderr, "[Server] Bind error: %s\n", strerror(errno));
        return (-1);
    }
    printf("[Server] Bound socket to localhost port %d\n", 4242);
    return (socket_fd);
}

void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size) 
{   
    if (*poll_count == *poll_size) {
        *poll_size *= 2;
        *poll_fds = (pollfd*)realloc(*poll_fds, sizeof(**poll_fds) * (*poll_size));
        if (!*poll_fds) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
    }

    (*poll_fds)[*poll_count].fd = new_fd;
    (*poll_fds)[*poll_count].events = POLLIN;
    (*poll_count)++;
}

void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count) {
    // Copie le fd de la fin du tableau à cet index
    (*poll_fds)[i] = (*poll_fds)[*poll_count - 1];
    (*poll_count)--;
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


// Lit le message d'une socket et relaie le message à toutes les autres
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
    printf("---- SERVER ----\n\n");

    int server_socket;
    int status;

    // Pour surveiller les sockets clients :
    struct pollfd *poll_fds; // Tableau de descripteurs
    int poll_size; // Taille du tableau de descipteurs
    int poll_count; // Nombre actuel de descripteurs dans le tableau

    // Création de la socket du serveur
    server_socket = create_server_socket();
    if (server_socket == -1) {
        return (1);
    }

    // Écoute du port via la socket
    printf("[Server] Listening on port %d\n", 4242);
    status = listen(server_socket, 10);
    if (status != 0) {
        fprintf(stderr, "[Server] Listen error: %s\n", strerror(errno));
        return (3);
    }

    // Préparation du tableau des descripteurs de fichier pour poll()
    // On va commencer avec assez de place pour 5 fds dans le tableau,
    // on réallouera si nécessaire
    poll_size = 5;
    poll_fds = (pollfd*)calloc(poll_size + 1, sizeof *poll_fds);
    if (!poll_fds) {
        return (4);
    }

    // Ajoute la socket du serveur au tableau
    // avec alerte si la socket peut être lue
    poll_fds[0].fd = server_socket;
    poll_fds[0].events = POLLIN;
    poll_count = 1;

    printf("[Server] Set up poll fd array\n");

    while (1) { // Boucle principale

        // Sonde les sockets prêtes (avec timeout de 2 secondes)
        status = poll(poll_fds, poll_count, 2000);
        if (status == -1) {
            fprintf(stderr, "[Server] Poll error: %s\n", strerror(errno));
            exit(1);
        }
        else if (status == 0) {
            printf("[Server] Waiting...\n");
            continue;
        }

        for (int i = 0; i < poll_count; i++) {
            if ((poll_fds[i].revents & POLLIN) != 1) {
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

