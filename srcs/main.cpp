#include "irc.hpp"

int main(int argc, char **argv)
{
    if (argc != 2){
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return 1; 
    }
    (void)argv;

    server();

    return 0;
}
