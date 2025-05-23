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

    // client->Send_message("Password valid, welcome");

    if (client->_registred_user && client->_registred_password == 0 && !client->get_nick().empty())
        MessageRegister(client);
            
    client->_registred_password = 1;
    
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

    

    if (client->_registred_user && client->_registred_password && client->get_nick().empty())
    {
        client->SET_Nick(argument[0]);
        MessageRegister(client);
    }
    else
        client->SET_Nick(argument[0]);

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
    
    if (client->_registred_user == 0 && client->_registred_password == 1 && !client->get_nick().empty())
        MessageRegister(client);
    client->_registred_user = 1;
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

    if (argument.size() < 1)
        return ERR(client, 461, "JOIN", "Not enough parameters");
    
    std::vector<std::string> Names = split(argument[0], ',');
    std::vector<std::string> Keys;

    if (argument.size() >= 2)
        Keys = split(argument[1], ',');

    std::string channel_name;
    std::string msg;
    std::string key;
    Channel     *channel;

    for (size_t i = 0; i < Names.size(); i++)
    {
        channel_name = Names[i];
        key = Keys.size() > i ? Keys[i] : "";
        
        if (channel_name[0] != '#' && channel_name[0] != '&') 
        {
            ERR(client, 476, channel_name, "Bad Channel Mask");
            continue;
        }

        if (channel_name.length() > 50)
        {
            ERR(client, 403, client->get_nick() + " " + channel_name , "No such channel");
            continue;
        }

        channel = CHANNEL_Exist(channel_name);
    
        if (!channel)
        {
            channel = new Channel(channel_name, "", client);
            // channel->SET_Owner(client);
            _channels.push_back(channel);
        }
        else if (channel->Try_Join(client, key)) // Probleme pour entrer dans le channel  full/invite/password
            continue;

        if (channel->Add_User(client)) // already in the channel
            continue;

        /// JOIN MSG 
        client->Join_Channel(channel);
        msg = ":" + client->get_Prefix() + " JOIN :" + channel_name;
        channel->New_User_msg(msg);


        // SEND TOPIC 331 : NO TOPIC || 332 : TOPIC
        if (channel->GET_Topic().empty())
            msg = ":localhost 331 " + client->get_nick() + " " + channel_name + " :No topic is set";
        else
            msg = ":localhost 332 " + client->get_nick() + " " + channel_name + " :" + channel->GET_Topic();
        client->Send_message(msg);


        // SEND NAMES / @ for secret channels / * for private channels / = for others (public channels).
        //              ou alors @ pour l'operateur
        msg = ":localhost 353 " + client->get_nick() + " = " + channel_name + " :@" + channel->ClientList();
        client->Send_message(msg);

        // SEND END OF NAMES
        msg = ":localhost 366 " + client->get_nick() + " " + channel_name + " :End of NAMES list";
        client->Send_message(msg);
    }
    

    ////////////
    // KEYS   //
    ////////////
    
    
}


void Server::MessageRegister(Client *client)
{
    std::string msg;
    
    msg = ":localhost 001 " + client->get_nick() + " :Welcome to the ft_irc network " + client->get_Prefix() + "";
    client->Send_message(msg);
    
    msg = ":localhost 002 " + client->get_nick() + " :Your host is localhost, running version 1.0";
    client->Send_message(msg);

    msg = ":localhost 003 " + client->get_nick() + " :This server was created in 2025"; 
    client->Send_message(msg);
    
    msg = ":localhost 004 " + client->get_nick() + " localhost 1.0 : o o";
    client->Send_message(msg);
    
    msg = ":localhost 375 " + client->get_nick() + " :- ft_irc Message of the day -";
    client->Send_message(msg);
    
    msg = ":localhost 372 " + client->get_nick() + " :Welcome to the ircsev server";
    client->Send_message(msg);
    
    msg = ":localhost 376 " + client->get_nick() + " :End of MOTD command";
    client->Send_message(msg);
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


void Server::INVITE(Client *client, std::vector<std::string> argument)
{
    std::cout << "INVITE DETECTED" << std::endl;

    if (argument.size() < 2)
        return ERR(client, 461, "INVITE", "Not enough parameters");

    std::string new_client_name = argument[0];
    std::string channel_name = argument[1];

    Client *new_client = this->FINDING_Client_str(new_client_name); // CHECK CLIENT EXIST
    if (!new_client)
        return ERR(client, 401, new_client_name, "No such nick/channel");

    Channel *channel = CHANNEL_Exist(channel_name); // CHECK CHANNEL EXIST
    if (!channel)
        return ERR(client, 403, channel_name, "No such channel");
    
    if (channel->Client_in_Channel(client->get_nick()) == 0) // CHECK IF CLIENT WHO SENT MESSAGE IS IN THE CHANNEL
        return ERR(client, 442, channel_name, "You're not on that channel");
    
    if (channel->Client_in_Channel(new_client_name) == 1) // CHECK IF NEW CLIENT IS IN THE CHANNEL
        return ERR(client, 443, new_client_name + " " + channel_name, "is already on channel ");
    
    if (channel->Try_Invite(client, new_client)) // ADD NEW CLIENT TO THE INVITE LIST
        return ERR(client, 482, channel_name, "You're not channel operator");

    
    // REPLY TO THE CLIENT WHO SENT THE INVITE
    std::string msg = ":localhost 341" + client->get_nick() + " " + new_client_name + " :" + channel_name;
    client->Send_message(msg);


    // INVITE MESSAGE TO THE NEW CLIENT
    msg = ":" + client->get_Prefix() + " INVITE " + new_client_name + " " + channel_name;
    new_client->Send_message(msg);

}


void Server::MODE(Client *client, std::vector<std::string> argument)
{
    std::cout << "MODE DETECTED" << std::endl;

    if (argument.size() < 1)
        return ERR(client, 461, "MODE", "Not enough parameters");

    std::string channel_name = argument[0];
    std::string msg;
    
    Channel *channel = CHANNEL_Exist(channel_name); // CHECK CHANNEL EXIST
    if (!channel)
        return ERR(client, 403, channel_name, "No such channel");
    
    if (channel->Client_in_Channel(client->get_nick()) == 0) // CHECK IF CLIENT IS IN THE CHANNEL
        return ERR(client, 442, channel_name, "You're not on that channel");
    
    if (channel->Is_Operator(client) == 0) // CHECK IF CLIENT IS OPERATOR
        return ERR(client, 482, channel_name, "You're not channel operator");
    
    if (argument.size() == 1) // MODE WITHOUT PARAMETER RETURN ALL THE MODE
    {
        msg = ":localhost 324 " + client->get_nick() + " " + channel_name;
        msg += "+ i";
        // msg += channel->GET_Mode_List();
        client->Send_message(msg);
        return;
    }

    std::string mode = argument[1];
    
    if (mode == "+i")
    {
        channel->SET_Owner(client);
        channel->SET_Topic("secret");
    }
    else if (mode == "-i")
    {
        channel->SET_Owner(NULL);
        channel->SET_Topic("");
    }
    else if (mode == "+k")
    {
        if (argument.size() < 3)
            return ERR(client, 461, "MODE", "Not enough parameters");
        // channel->SET_Password(argument[2]);
    }
    else if (mode == "-k")
    {
        // channel->SET_Password("");
    }
    else if (mode == "+l")
    {
        if (argument.size() < 3)
            return ERR(client, 461, "MODE", "Not enough parameters");
        // channel->SET_Max_User(std::atoi(argument[2].c_str()));
    }
    else if (mode == "-l")
    {
        // channel->SET_Max_User(0);
    }
    else if (mode == "+t")
    {
        // channel->SET_Topic("secret");
    }
    else if (mode == "-t")
    {
        // channel->SET_Topic("");
    }
    else if (mode == "+o")
    {
        if (argument.size() < 3)
            return ERR(client, 461, "MODE", "Not enough parameters");
        Client *target_client = this->FINDING_Client_str(argument[2]);
        if (!target_client)
            return ERR(client, 401, argument[2], "No such nick/channel");
        channel->Try_Invite(client, target_client);
    }
    else if (mode == "-o")
    {
        if (argument.size() < 3)
            return ERR(client, 461, "MODE", "Not enough parameters");
        Client *target_client = this->FINDING_Client_str(argument[2]);
        if (!target_client)
            return ERR(client, 401, argument[2], "No such nick/channel");
        channel->Try_Invite(client, target_client);
    }
    else
        return ERR(client, 472, mode, "is unknown mode char to me for /channel");

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
