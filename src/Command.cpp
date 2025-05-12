#include "Server.hpp"

int Server::NICK_Already_Exist(std::string nickname)
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
    {
        if ((*it)->get_nick() == nickname)
            return 1;
    }
    return 0;
}

void Server::PASS(Client *client, std::vector<std::string> argument)
{
    std::cout << "PASS DETECTED" << std::endl;
    (void)client;
    (void)argument;
    if (argument.size() != 1)
        return ERR(client, 461, "PASS", "Not enough parameters");
    if (client->_registred_password == 1)
    {
        return ERR(client, 461, "", ":Unauthorized command (already registered)");
        return ;
    }

    if (argument[0] == _password)
    {
        client->_registred_password = 1;
        client->Send_message("Password valid, welcome");
    }
    else
        return ERR(client, 461, "","Password incorrect");
}

void Server::NICK(Client *client, std::vector<std::string>argument)
{
    std::cout << "NICK DETECTED" << std::endl;

    if (argument[0].empty())
        return ERR(client, 1, "", "nickname empty");
    if (NICK_Already_Exist(argument[0]))
        return ERR(client, 460, "NICK", "Nick already exist");

    //if (too much arg)
        //
    client->SET_Nick(argument[0]);
    client->Send_message("Nick valid, welcome " + argument[0]);
    if (client->REGISTRED())
    {
        client->_registred_user = 1;
        client->Send_message("You are entierly registered ! Welcome on IRC");
    }
}

void Server::USER(Client *client, std::vector<std::string>argument)
{
    std::cout << "USER DETECTED" << std::endl;

    if (argument.size() != 4)
        return ERR(client, 1, "", "arg nb wrong");
    if (argument[0].empty())
        return ERR(client, 1, "", "username empty");
    if (argument[1].size() != 1 || !std::isdigit(argument[1][0]))
        return ERR(client, 1, "", "mode isnt numeric or too long [0 - 9]");
    if (argument[0].size() > 9)
        return ERR(client, 1, "", "username too long");
    //if (username already exist)

    client->SET_Username(argument);
    client->Send_message("Username valid, welcome" + argument[0]);
    if (client->REGISTRED())
    {
        client->_registred_user = 1;
        client->Send_message("You are entierly registered ! Welcome on IRC");
    }
}

void Server::QUIT(Client *client, std::vector<std::string>argument)
{
    std::cout << "QUIT DETECTED" << std::endl;

    (void)client;
    (void)argument;
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

void Server::JOIN(Client *user, std::vector<std::string> argument)
{
    std::cout << "JOIN DETECTED" << std::endl;
    std::string msg;
    std::string channel_name = argument[0];
    int status;

    if (channel_name.empty())
    {
        std::cerr << "No channel specified" << std::endl;
        return;
    }

    Channel *channel = CHANNEL_Exist(channel_name);
    if (!channel)
    {
        std::string topic = "No topic\n";
        channel = new Channel(channel_name, topic);
        _channels.push_back(channel);
        msg = "You created " + channel_name + " and you are now an administrator\n";
        status = send(user->get_clientfd(), msg.c_str(), msg.size(), 0);
        user->OPERATOR();
    }
    if (user->get_channel() != channel)
    {
        std::cout << "Channel " << channel_name << " joined by " << user->get_username() << std::endl;
        channel->ADD_User(user);
        msg = "You Joined " + channel_name + "\n";
        status = send(user->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    else
    {
        std::cout << user->get_username() << " already joined " << channel->GET_Name() << std::endl;
        msg = "You already joined " + channel_name + "\n";
        status = send(user->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
   (void)status;
}
void Server::KICK(Client *client, std::vector<std::string> argument)
{
    std::cout << "KICK DETECTED ON " << argument[0] << std::endl;
    std::string msg;
    int status;

    if (!client->is_operator())
        return;
    Client *kicked_user = FINDING_Client_str(argument[0]);
    if (kicked_user && kicked_user->get_channel() == client->get_channel())
    {
        kicked_user->get_channel()->DELETE_User(kicked_user);
        msg = "Client " + kicked_user->get_username() + " has benn kicked of the " + client->get_channel()->GET_Name() + " channel\n";
        status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
        msg = "You have been kickend from " + client->get_channel()->GET_Name() + " by " + client->get_username() + "\n";
        status = send(kicked_user->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    else if (kicked_user && kicked_user->get_channel() != client->get_channel())
    {
        msg = "Client " + kicked_user->get_username() + " is not in your channel " + client->get_channel()->GET_Name() + "\n";
        status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    else
    {
        msg = "No client " + argument[0] + " found\n";
        status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    (void)status;
    (void)kicked_user;
}
void Server::INVITE(Client *client, std::vector<std::string> argument)
{
    std::cout << "INVITE DETECTED ON " << argument[0] << std::endl;
    if (!client->is_operator())
        return;
}
void Server::TOPIC(Client *client, std::vector<std::string> argument)
{
    std::cout << "TOPIC DETECTED" << std::endl;;
    std::string msg;
    int status;
    if (client->get_channel() && argument.empty())
    {
        msg = client->get_channel()->GET_Topic();
        status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    else if (argument.empty())
    {
        msg = "No channel joined yet\n";
        status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    else if  (!client->is_operator())
    {
        msg = "You are not habilited to change the topic of this channe\n";
        status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    else
    {
        client->get_channel()->SET_Topic(argument[0]);
        msg = "Topic successfully changed !\n";
        status = send(client->get_clientfd(), msg.c_str(), msg.size(), 0);
    }
    (void)status;
    (void)argument;
}

void Server::MODE(Client *client, std::vector<std::string> argument)
{
    std::cout << "MODE " << argument[0] << " DETECTED" << std::endl;
    if (!client->is_operator())
        return;
}