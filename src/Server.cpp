#include "Server.hpp"
#include "Client.hpp"

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
    for (size_t i = 0; i < _clients.size(); i++)
    {
        delete _clients[i];
    }
    
    std::cout << "Destructor Server called" << std::endl; 
}


void Server::disconect_client(Client *client)
{
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
        if (it->fd == client->get_clientfd()) {
            _poll_fds.erase(it);
            break;
        }
    }

    for (std::size_t i = 0; i < _clients.size(); i++){
        if (_clients[i]->get_clientfd() == client->get_clientfd())
        {
            // maybe close de fd
            close(client->get_clientfd());
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
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

    std::string msg = "Welcome. You are client " + client->get_username() + " " + client->get_nick() + "\n";
    int status = send(client_fd, msg.c_str(), msg.size(), 0);
    if (status == -1)
        std::cerr << "[Server] Send error to client " << client_fd << ": " << strerror(errno) << std::endl;
}


void Server::read_data_from_socket(Client *client)
{
    char buffer[1024];
    
    int bytes_read = recv(client->get_clientfd(), buffer, sizeof(buffer) - 1, 0);
    if (bytes_read < 0) { // && errno != EWOULDBLOCK
        std::cerr << "[Server] Recv error: " << strerror(errno) << std::endl;
        this->disconect_client(client);
        return ;
    }

    if (bytes_read == 0){
        std::cout << "[Server] " << client->get_nick() << ", client fd " << client->get_clientfd() << " disconnected" << std::endl;
        this->disconect_client(client);
        return ;
    }
    
    buffer[bytes_read] = 0;

    std::string& msg = client->get_message();
    msg.append(buffer);
    
    while (!msg.empty()) {
        size_t position = msg.find('\n');
    
        if (position == std::string::npos) {
            return;
        }
    
        std::string cmd(msg.c_str(), position);
        if (cmd[cmd.size() - 1] == '\r')
            cmd.erase(cmd.size() - 1);
        // std::cout << "size cmd : " << cmd.size() << " : " << cmd << std::endl;
        
        IRC_Parser(cmd, this, client);
        msg.erase(0, position + 1);
    }

    // int continu = 0;
    // do {
    //     size_t position = msg.find('\n');

    //     if (position == std::string::npos){
    //         return;
    //     }
    //     std::string cmd(msg.c_str(), position);
    //     std::cout << cmd << std::endl;
    //     // IRC_Parser(msg, this, client);

    //     ++position;

    //     if (position < msg.length()){
    //         continu = 1;
    //         msg = msg.erase(0, position);
    //     }
    //     else
    //         continu = 0;

    // } while (continu);
    // msg.clear();

    

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
        if (_clients[i]->get_username() == username)
            return _clients[i];
    }
    return NULL;
}


void	Server::start()
{
	std::cout << "---- SERVER ----\n" << std::endl;

    pollfd server_fd = {_server_socket, POLLIN, 0};
    _poll_fds.push_back(server_fd);
    while (1) 
    {    
        int status = poll(_poll_fds.begin().base(), _poll_fds.size(), -1);
        
        if (status == -1) {
            std::cerr << "[Server] Poll error: " << strerror(errno) << std::endl;
            exit(1);
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

// if (it->revents & POLLERR) {
//     std::cerr << "[Server] Error on fd: " << it->fd << std::endl;
//     close(it->fd);
//     (&_poll_fds, it - _poll_fds.begin(), &_poll_fds.size());
//     continue;
// }


/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    COMMANDE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

//REGISTER
void Server::PASS(Client *client, std::vector<std::string> argument)
{
    std::cout << "PASS DETECTED" << std::endl;
    (void)client;
    (void)argument;
    if (argument.size() != 1)
        err_461("PASS", client);
    if (client->_registred_password == 1)
    {
        err_462(client);
        return ;
    }

    if (argument[0] == _password)
        client->_registred_password = 1;
    // else
    //     err_464(client);
}

void Server::NICK(Client *client, std::vector<std::string>argument)
{
    std::cout << "NICK DETECTED" << std::endl;

    //if (argument[0].empty)
        //ERR_NONICKNAMEGIVEN
    //if (ncik already exist)
        //
    //if (too long)
        //
    //if (too much arg)
        //
    client->SET_Nick(argument[0]);
    if (client->REGISTRED())
        client->_registred_user = 1;
}
void Server::USER(Client *client, std::vector<std::string>argument)
{
    std::cout << "NICK DETECTED" << std::endl;

    //if (argument[0].empty)
        //ERR_NONICKNAMEGIVEN
    //if (ncik already exist)
        //
    //if (too long)
        //
    //if (too much arg)
        //
    client->SET_Nick(argument[0]);
    if (client->REGISTRED())
        client->_registred_user = 1;
}
// void Server::QUIT(Client *client, std::vector<std::string>argument)
// {
//     std::cout << "QUIT DETECTED" << std::endl;


// }


Channel *Server::CHANNEL_Exist(std::string channel_name)
{
    for (size_t i = 0; i < _channels.size(); ++i)
    {
        if (_channels[i]->GET_Name() == channel_name)
            return _channels[i];
    }
    return NULL;
}



void Server::JOIN(Client *client, std::vector<std::string> argument)
{
    std::cout << "JOIN DETECTED" << std::endl;
    std::string msg;
    std::string channel_name = argument[0];
    int status;

    if (channel_name.empty())
    {
        // sendError(client, 461, "JOIN", "Not enough parameters");
        return;
    }

    Channel *channel = CHANNEL_Exist(channel_name);

    if (!channel)
    {
        std::string topic = "No topic\n";

        channel = new Channel(channel_name, topic);
        _channels.push_back(channel);
    }
    
    
    if (channel->Add_User(client))
    {
        // already in the channel
        return;
    }

    client->Join_Channel(channel);
    std::string joinMsg = ":" + client->get_Prefix() + " JOIN :" + channel_name;
    channel->New_User_msg(msg);

    // if (client->get_channel() != channel)
    // {
    //     std::cout << "Channel " << channel_name << " joined by " << client->get_username() << std::endl;
    //     channel->ADD_User(client);
    //     msg = "You Joined " + channel_name + "\n";
    //     status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    // }
    // else
    // {
    //     std::cout << client->get_username() << " already joined " << channel->GET_Name() << std::endl;
    //     msg = "You already joined " + channel_name + "\n";
    //     status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    // }
   (void)status;
}




// void Server::KICK(Client *client, std::vector<std::string> argument)
// {
//     std::cout << "KICK DETECTED ON " << argument[0] << std::endl;
//     std::string msg;
//     int status;

//     if (!client->is_operator())
//         return;
//     Client *kicked_user = FINDING_Client_str(argument[0]);
//     if (kicked_user && kicked_user->get_channel() == client->get_channel())
//     {
//         kicked_user->get_channel()->DELETE_User(kicked_user);
//         msg = "Client " + kicked_user->get_username() + " has benn kicked of the " + client->get_channel()->GET_Name() + " channel\n";
//         status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
//         msg = "You have been kickend from " + client->get_channel()->GET_Name() + " by " + client->get_username() + "\n";
//         status = send(kicked_user->get_clientfd(), msg.c_str(), msg.size(), 0);
//     }
//     else if (kicked_user && kicked_user->get_channel() != client->get_channel())
//     {
//         msg = "Client " + kicked_user->get_username() + " is not in your channel " + client->get_channel()->GET_Name() + "\n";
//         status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
//     }
//     else
//     {
//         msg = "No client " + argument[0] + " found\n";
//         status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
//     }
//     (void)status;
//     (void)kicked_user;
// }
void Server::INVITE(Client *client, std::vector<std::string> argument)
{
    std::cout << "INVITE DETECTED ON " << argument[0] << std::endl;
    if (!client->is_operator())
        return;
}
// void Server::TOPIC(Client *client, std::vector<std::string> argument)
// {
//     std::cout << "TOPIC DETECTED" << std::endl;;
//     std::string msg;
//     int status;
//     if (client->get_channel() && argument.empty())
//     {
//         msg = client->get_channel()->GET_Topic();
//         status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
//     }
//     else if (argument.empty())
//     {
//         msg = "No channel joined yet\n";
//         status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
//     }
//     else if  (!client->is_operator())
//     {
//         msg = "You are not habilited to change the topic of this channe\n";
//         status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
//     }
//     else
//     {
//         client->get_channel()->SET_Topic(argument[0]);
//         msg = "Topic successfully changed !\n";
//         status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
//     }
//     (void)status;
//     (void)argument;
// }
void Server::MODE(Client *client, std::vector<std::string> argument)
{
    std::cout << "MODE " << argument[0] << " DETECTED" << std::endl;
    if (!client->is_operator())
        return;
}
