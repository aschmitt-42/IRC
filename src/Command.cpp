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
    if (argument.size() != 1)
        return ERR(client, 461, "PASS", "Not enough parameters");
    if (client->_registred_password == 1)
    {
        return ERR(client, 462, "", ":Unauthorized command (already registered)");
        return ;
    }

    if (argument[0] != _password)
        return ERR(client, 464, "","Password incorrect");

    client->_registred_password = 1;
    client->Send_message("Password valid, welcome");
    if (client->REGISTRED())
        MessageRegister(client);
    
}

void Server::NICK(Client *client, std::vector<std::string>argument)
{
    std::cout << "NICK DETECTED" << std::endl;
    
    if (argument.size() < 1)
        return ERR(client, 431, "", "No nickname given");

    if (NICK_Already_Exist(argument[0]))
        return ERR(client, 433, argument[0], "Nickname is already in use");

    // verifier si le nickname est bon (pas de caractere interdit)
    // ERR_ERRONEUSNICKNAME : "432 <nick> :Erroneous nickname\r\n"

    
    // if (client->get_nick() != "") --> a deja un nick donc le change
    //      envoyer message comme quoi il a changer de nick a tout les client register

    client->SET_Nick(argument[0]);

    if (client->REGISTRED())
        MessageRegister(client);

    // PAS SUR client->Send_message("Nick valid, welcome " + argument[0]);

}

void Server::USER(Client *client, std::vector<std::string>argument)
{
    std::cout << "USER DETECTED" << std::endl;
    
    if (argument.size() != 4)
        return ERR(client, 461, "USER", "Not enough parameters");
    
    if (client->_registred_user == 1)
        return ERR(client, 462, "", "Unauthorized command (already registered)");

    /*
    VERIF USERNAME --> ERR_ERRONEUSUSERNAME : "491 <user> :Erroneous username"

    if (argument[0].empty() || argument[0].size() > 9)
        return ERR(client, 432, "USER", "Erroneous nickname");

    if ()

    */

    // PAS SUR 
    // if (argument[1].size() != 1 || !std::isdigit(argument[1][0]))
    //    return ERR(client, 1, "", "mode isnt numeric or too long [0 - 9]");
    
    // if (username already exist)

    client->SET_Username(argument);
    // client->Send_message("Username valid, welcome" + argument[0]);
    client->_registred_user = 1;
    if (client->REGISTRED())
        MessageRegister(client);
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

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

std::vector<std::string> split(const std::string input, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, delimiter)) 
    {
        // Supprimer les espaces éventuels autour du token
        token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
        if (!token.empty()) {
            result.push_back(token);
        }
    }

    return result;
}

void Server::JOIN(Client *client, std::vector<std::string> argument)
{
    std::cout << "JOIN DETECTED" << std::endl;
    
    std::vector<std::string> Names = split(argument[0], ',');

    std::string channel_name;
    std::string msg;
    Channel     *channel;

    for (size_t i = 0; i < Names.size(); i++)
    {
        channel_name = Names[i];

        //  VERIF NOM DE CHANNEL
        if (channel_name.empty())
        {
            ERR(client, 461, "JOIN", "Not enough parameters");
            continue;
        }

        channel = CHANNEL_Exist(channel_name);
    
        if (!channel)
        {
            channel = new Channel(channel_name, "");
            _channels.push_back(channel);
        }


        if (channel->Add_User(client))
        {
            // already in the channel
            continue;
        }

        /// JOIN MSG 
        client->Join_Channel(channel);
        msg = ":" + client->get_Prefix() + " JOIN :" + channel_name ;
        channel->New_User_msg(msg);


        // SEND TOPIC 331 : NO TOPIC || 332 : TOPIC
        if (channel->GET_Topic().empty())
            msg = ":localhost 331 " + client->get_nick() + " " + channel_name + " :No topic is set";
        else
            msg = ":localhost 332 " + client->get_nick() + " " + channel_name + " :" + channel->GET_Topic();
        client->Send_message(msg);


        // SEND NAMES / @ for secret channels / * for private channels / = for others (public channels).
        msg = ":localhost 353 " + client->get_nick() + " = " + channel_name + " :@" + channel->ClientList();
        client->Send_message(msg);
        std::cout << "NAMES : " << msg << std::endl;

        // SEND END OF NAMES
        msg = ":localhost 366 " + client->get_nick() + " " + channel_name + " :End of NAMES list";
        client->Send_message(msg);
    }
    

    ////////////////////////////////////////////////////////////////////////////
    // TOPIC, KEYS, PLUSIEURS CHANNELS
    ////////////////////////////////////////////////////////////////////////////
    
    
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


void Server::MessageRegister(Client *client)
{
    std::string msg;

    msg = ":localhost 001 " + client->get_nick() + " :Welcome to the ft_irc network " + client->get_Prefix() + "";
    client->Send_message(msg);

    msg = ":localhost 002 " + client->get_nick() + " :Your host is localhost, running version 1.0";
    client->Send_message(msg);

    msg = ":localhost 003 " + client->get_nick() + " :This server was created 2023-10-01";
    client->Send_message(msg);

    msg = ":localhost 004 " + client->get_nick() + " localhost 1.0 : o o";
    client->Send_message(msg);
    
    msg = ":localhost 375 " + client->get_nick() + " :- ft_irc Message of the day -";
    client->Send_message(msg);
    
    msg = ":localhost 372 " + client->get_nick() + " :Welcome to the ft_irc server";
    client->Send_message(msg);
    
    msg = ":localhost 376 " + client->get_nick() + " :End of MOTD command";
    client->Send_message(msg);

    /*
    Message de registration
    :localhost 001 <nick> :Welcome to the ft_irc network
    :localhost 002 <nick> :Your host is localhost, running version 1.0
    :localhost 003 <nick> :This server was created 2023-10-01
    :localhost 004 <nick> localhost 1.0 :<user modes> <channel modes>
    375
    372
    376
    */
}

void Server::PING(Client *client, std::vector<std::string> argument)
{
    std::cout << "PING DETECTED" << std::endl;
    std::string msg;
    if (argument.size() < 1)
        msg = ":localhost PONG :";
    else
        msg = ":localhost PONG :" + argument[0];
    client->Send_message(msg);
}

void Server::PRIVMSG(Client *client, std::vector<std::string> argument)
{
    std::string msg;

    // if (argument.size() < 2)
    //     return ERR(client, 461, "PRIVMSG", "Not enough parameters");
    
    if (argument[0][0] == '#') // ou autre caractere accepter de debut de channel
    {
        std::cout << "PRIVMSG TO CHANNEL "<< std::endl;
        Channel *channel = CHANNEL_Exist(argument[0]);
        if (!channel)
            return ERR(client, 403, argument[0], "No such channel");
        msg = ":" + client->get_nick() + "!" + client->get_username() + "@" + "localhost";
        msg += " PRIVMSG " + argument[0] + " " + argument[1];
        channel->SEND_Msg(msg, client);
    }
    else
    {
        std::cout << "PRIVMSG DIRECT TO ANOTHER CLIENT "<< std::endl;
        Client *target_client = FINDING_Client_str(argument[0]);
        if (!target_client)
            return ERR(client, 401, argument[0], "No such nick/channel");
        msg = ":" + client->get_Prefix() + " PRIVMSG " + target_client->get_nick() + " :" + argument[1];
        client->Send_message(msg);
    }
}