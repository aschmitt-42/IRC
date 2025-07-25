
#include "Server.hpp"

int     IS_Client_In_Vector(std::vector<Client*> client_vector, Client *client)
{
    for (size_t i = 0; i < client_vector.size(); ++i)
    {
        if (client->get_nick() == client_vector[i]->get_nick())
            return 1;
    }
    return 0;
}

std::vector<ModChange> MODE_Parser(Client *client, std::vector<std::string> argument) 
{


    std::vector<ModChange> result;
    std::string modeString = argument[1];
    size_t argIndex = 2;
    bool adding = true; // '+' ou '-'
    char c;

    for (size_t i = 0; i < modeString.size(); ++i) 
    {
        c = modeString[i];
        if (c == '+') 
            adding = true;
        else if (c == '-')
            adding = false;
        else 
        {
            if (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l')
            {
                 result.clear();
                 return (result);
            }
            ModChange modeChange;
            modeChange.mode = c;
            modeChange.add = adding;

            if (c == 'k' || c == 'o' || (c == 'l' && adding)) 
            {
                if (argIndex >= argument.size()) 
                {
                    result.clear();
                    ERR(client, 461, "MODE", "Not enough parameters");
                    return (result);
                }
                else
                    modeChange.argument.push_back(argument[argIndex++]);
            }
            else
                modeChange.argument.push_back("");
            result.push_back(modeChange);
        }
    }
    return result;
}


Channel *Server::CHANNEL_Exist(std::string channel_name)
{
    for (size_t i = 0; i < _channels.size(); ++i)
    {
        if (_channels[i]->GET_Name() == channel_name)
            return _channels[i];
    }
    return NULL;
}

int Server::NICK_Already_Exist(std::string nickname)
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
    {
        if ((*it)->get_nick() == nickname)
            return 1;
    }
    return 0;
}

std::vector<std::string> split(const std::string input, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, delimiter)) 
    {
        token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
        if (!token.empty()) {
            result.push_back(token);
        }
    }

    return result;
}

int create_server_socket(int port) {

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

    fcntl(socket_fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET; // IPv4
    sockaddr.sin_addr.s_addr = INADDR_ANY;
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
    std::string err_msg;
	if (!client)
        return;
    
	if (command.empty())
        err_msg = ":localhost " + intToString(err_id) + " :" + msg;
	else
        err_msg = ":localhost " + intToString(err_id) + " " + command + " :" + msg;
	
    client->Send_message(err_msg);
}

bool isValidNick(const std::string& nick) 
{
    char c;

    if (nick.empty() || nick.length() > 9) 
        return false;
    if (!isalpha(nick[0]))
        return false;

    for (size_t i = 1; i < nick.length(); i++) 
    {
        c = nick[i];
        if (!isalnum(c) && c != '-' && c != '[' && c != ']' && c != '\\' && c != '`' && c != '^' && c != '{' && c != '}') 
            return false;
    }
    return true;
}
