#include "Server.hpp"
#include "Client.hpp"

void verif_port(std::string port)
{
    if (port.empty())
        std::cerr << "Invalid port" << std::endl;
    
    for (size_t i = 0; i < port.length(); ++i)
        if (!std::isdigit(port[i]))
            std::cerr << "Invalid port" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 3){
        std::cerr << "Usage: ./ircserv port password" << std::endl;
        return 1; 
    }
    
    verif_port(argv[1]);
    Server irc(argv[1], argv[2]);

    irc.start();

    return 0;
}
