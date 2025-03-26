// showip.c -- un simple programme qui montre la ou les adresses IP d'un domaine
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main(int ac, char **av) {
    struct addrinfo hints; // Indications pour getaddrinfo()
    struct addrinfo *res;  // Résultat de getaddrinfo()
    struct addrinfo *r;    // Pointeur pour itérer sur les résultats
    int status; // Valeur de retour de getaddrinfo()
    char buffer[INET6_ADDRSTRLEN]; // Buffer pour reconvertir l'adresse IP

    if (ac != 2) {
        fprintf(stderr, "usage: /a.out hostname\n");
        return (1);
    }

    memset(&hints, 0, sizeof hints); // Initialise la structure
    hints.ai_family = AF_UNSPEC; // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Récupère la ou les adresses IP associées
    status = getaddrinfo(av[1], 0, &hints, &res);
    if (status != 0) { // en cas d'erreur
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return (2);
    }

    printf("IP adresses for %s:\n", av[1]);

    r = res;
    int i = 0;
    while (r != NULL) {
        printf("i = %d\n", i++);
        void *addr; // Pointeur vers l'adresse IP
        if (r->ai_family == AF_INET) { // Adresse IPv4
            // il faut caster l'adresse en structure sockaddr_in pour récupérer
            // l'adresse IP, comme le champ ai_addr pourrait être soit
            // un sockaddr_in soit un sockaddr_in6
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)r->ai_addr;
            // Transforme l'entier en adresse IP lisible
            inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof buffer);
            printf("IPv4: %s\n", buffer);
        } else { // Adresse IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)r->ai_addr;
            inet_ntop(r->ai_family, &(ipv6->sin6_addr), buffer, sizeof buffer);
            printf("IPv6: %s\n", buffer);
        }
        r = r->ai_next; // Prochaine adresse renseignée par getaddrinfo()

    }
    freeaddrinfo(res); // Libère la mémoire
    return (0);
}

void    start()
{
    int status;
    int socket_fd;
    int client_fd;
    struct addrinfo hints;
    struct addrinfo *res;

    const char *PORT = "4242";
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    
    // on remplit hints pour préparer l'appel à getaddrinfo()
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // remplis l'IP automatiquement

    status = getaddrinfo(NULL, PORT, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return ;
    }

    // on crée la socket, on a lie et on écoute dessus
    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socket_fd == -1) {
        perror("socket");
        return ;
    }
    status = bind(socket_fd, res->ai_addr, res->ai_addrlen);
    if (status != 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return;
    }
    listen(socket_fd, BACKLOG);

    addr_size = sizeof client_addr;
    client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
    if (client_fd == -1) {
        fprintf(stderr, "accept: %s\n", strerror(errno));
        return;
    }
    printf("New connection! Socket fd: %d, client fd: %d\n", socket_fd, client_fd);


    (void)status;
    (void)socket_fd;
    (void)hints;
    (void)*res;

}

int test1(void)
{
    std::cout << "test" << std::endl;
    struct addrinfo hints;
    struct addrinfo *res;
    int socket_fd;
    int client_fd;
    int status;
    // sockaddr_storage est une structure qui n'est pas associé à
    // une famille particulière. Cela nous permet de récupérer
    // une adresse IPv4 ou IPv6
    struct sockaddr_storage client_addr;
    socklen_t addr_size;

    // on prépare l'adresse et le port pour la socket de notre serveur
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // IPv4 ou IPv6, indifférent
    hints.ai_socktype = SOCK_STREAM;    // Connexion TCP
    hints.ai_flags = AI_PASSIVE;        // Remplit l'IP automatiquement

    status = getaddrinfo("127.0.0.1", PORT, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return (1);
    }

    // on crée la socket, on a lie et on écoute dessus
    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    status = bind(socket_fd, res->ai_addr, res->ai_addrlen);
    if (status != 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return (2);
    }
    if (listen(socket_fd, BACKLOG) != 0) {
        perror("listen");
        return 4;
    }

    // on accept une connexion entrante
    addr_size = sizeof client_addr;
    std::cout << "accept" << std::endl;
    client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
    std::cout << "accept ok" << std::endl;
    if (client_fd == -1) {
        fprintf(stderr, "accept: %s\n", strerror(errno));
        return (3);
    }
    printf("New connection! Socket fd: %d, client fd: %d\n", socket_fd, client_fd);

    // on est prêts à communiquer avec le client via le client_fd !

    return (0);
}
