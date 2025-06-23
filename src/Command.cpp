#include "Server.hpp"

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

    if (isValidNick(argument[0]) == false)
        return ERR(client, 432, argument[0], "Erroneous nickname");
    
    if (client->get_nick() != "") // Change nick and message all clients
    {
        std::string msg = ":" + client->get_Prefix() + " NICK :" + argument[0];
        for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
            (*it)->Send_message(msg);
    }
    
    if (client->_registred_user && client->_registred_password && client->get_nick().empty())
    {
        client->SET_Nick(argument[0]);
        MessageRegister(client);
    }
    else
        client->SET_Nick(argument[0]);
}

void Server::USER(Client *client, std::vector<std::string>argument, std::string msg)
{
    std::cout << "USER DETECTED" << std::endl;
    
    if (argument.size() < 4)
        return ERR(client, 461, "USER", "Not enough parameters");
    
    if (client->_registred_user == 1)
        return ERR(client, 462, "", "Unauthorized command (already registered)");

    size_t tmp = msg.find(':');
    if (tmp != std::string::npos)
        msg = msg.substr(tmp + 1);
    else
        msg = argument[3];


    client->SET_Username(argument, msg); // RECUPERER REALNAME COMPLET
    
    // MESSAGE IF USER FINISH REGISTRATION
    if (client->_registred_user == 0 && client->_registred_password == 1 && !client->get_nick().empty())
        MessageRegister(client);
    client->_registred_user = 1;
}


void Server::QUIT(Client *client, std::string msg)
{
    std::cout << "QUIT DETECTED :" << std::endl;

    size_t tmp = msg.find(':');
    if (tmp != std::string::npos)
        msg = msg.substr(tmp + 1);
    else
        msg = "Leaving";

    client->SEND_Quit_Msg(msg); // send message to all clients who are in a channel with the client
    
    disconect_client(client);   // remove client from server 
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

        for (size_t i = 0; i < channel_name.size(); ++i)
        {
            if (channel_name[i] == ',' || channel_name[i] == ':'|| channel_name[i] == ' ' || channel_name[i] == '\0')
            {
                ERR(client, 403, client->get_nick() + " " + channel_name , "No such channel");
                continue;
            }
        }

        channel = CHANNEL_Exist(channel_name);
        if (!channel)
        {
            channel = new Channel(channel_name, "", client, this);
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
        channel->Send_Msg_To_All_Client(msg);


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

void Server::PRIVMSG(Client *client, std::vector<std::string> argument, std::string prv_msg)
{
    if (argument.size() < 2)
        return ERR(client, 461, "PRIVMSG", "Not enough parameters");

    std::string msg;
    std::string destination = argument[0];
    
    // ENLEVE LE PREFIX
    size_t first_space = prv_msg.find(' ');
    if (first_space != std::string::npos)
    {
        size_t second_space = prv_msg.find(' ', first_space + 1);
        if (second_space != std::string::npos)
        {
            prv_msg = prv_msg.substr(second_space + 1);
            if (prv_msg[0] == ':')
            {
                std::cout << "priv msg = " << prv_msg << std::endl;
                prv_msg = prv_msg.substr(1);
                std::cout << "apres priv msg = " << prv_msg << std::endl;

            }
        } 
        else
            prv_msg.clear();
    } 
    else
        prv_msg.clear();

    // if (prv_msg.empty())
    //     return ERR(client, 412, destination, "No text to send");

    if (destination[0] == '#' || destination[0] == '&')
    {
        std::cout << "PRIVMSG TO CHANNEL "<< std::endl;

        Channel *channel = CHANNEL_Exist(destination);
        if (!channel)
            return ERR(client, 403, destination, "No such channel");
        msg = ":" + client->get_Prefix() + " PRIVMSG " + destination + " :" + prv_msg;
        channel->SEND_Msg(msg, client);
    }
    else
    {
        std::cout << "PRIVMSG DIRECT TO ANOTHER CLIENT "<< std::endl;
        
        Client *target_client = FINDING_Client_str(destination);
        if (!target_client)
            return ERR(client, 401, destination, "No such nick/channel");
        msg = ":" + client->get_Prefix() + " PRIVMSG " + destination + " :" + prv_msg;
        target_client->Send_message(msg);
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
        return ERR(client, 482, client->get_nick() + " " + channel_name, "You're not channel operator");

    
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
    
    std::string msg;
    std::string channel_name = argument[0];

    Channel*    channel = CHANNEL_Exist(channel_name); // CHECK CHANNEL EXIST
    if (!channel)
        return ERR(client, 403, channel_name, "No such channel");

    if (channel->Client_in_Channel(client->get_nick()) == 0) // CHECK IF CLIENT IS IN THE CHANNEL
        return ERR(client, 442, channel_name, "You're not on that channel");

    if (argument.size() == 1)
    {
        std::cout << "MODE WITHOUT PARAMETER" << std::endl;

        msg = ":localhost 324 " + client->get_nick() + " " + channel_name + " :";
        msg += channel->GET_Mode_List();
        client->Send_message(msg);
        return;
    }
    
    if (channel->Is_Operator(client) == 0) // CHECK IF CLIENT IS OPERATOR
        return ERR(client, 482, client->get_nick() + " " + channel_name, "You're not channel operator");
    
    std::vector<ModChange> result = MODE_Parser(client, argument);
    std::cout << "END OF MODE PARSER " << std::endl;

    for (size_t i = 0; i < result.size(); ++i)
    {
        if (result[i].mode == 'i')
            channel->INVITE_Only(result[i].add, client);
        else if (result[i].mode == 't')
            channel->TOPIC_Restriction(client, result[i].add);
        else if (result[i].mode == 'k')
            channel->CHANGE_Pass(client, result[i].add, result[i].argument);
        else if (result[i].mode == 'o')
            channel->CHANGE_Operator(client, this, result[i].add, result[i].argument);
        else if (result[i].mode == 'l')
            channel->USER_Limit(client, result[i].add, result[i].argument);
        else
            std::cout << "WRONG MODE, ERROR : " << result[i].mode << std::endl;
    }
}

void Server::TOPIC(Client *client, std::vector<std::string> argument, std::string new_topic)
{
    std::cout << "TOPIC DETECTED" << std::endl;

    if (argument.size() < 1)
        return ERR(client, 461, "TOPIC", "Not enough parameters");

    std::string msg;
    std::string channel_name = argument[0];
    Channel *channel = CHANNEL_Exist(channel_name);
    if (!channel)
        return ERR(client, 403, channel_name, "No such channel");

    if (channel->Client_in_Channel(client->get_nick()) == 0) // CHECK IF CLIENT IS IN THE CHANNEL
        return ERR(client, 442, channel_name, "You're not on that channel");

    if (argument.size() == 1) // GET TOPIC
    {
        std::string topic = channel->GET_Topic();
        if (topic.empty())
            msg = ":localhost 331 " + client->get_nick() + " " + channel_name + " :No topic is set";
        else
            msg = ":localhost 332 " + client->get_nick() + " " + channel_name + " :" + topic;
        
        client->Send_message(msg);
        return;
    }

    if (channel->Is_Operator(client) == 0 && channel->Is_Topic_Restriction())
        return ERR(client, 482, client->get_nick() + " " + channel_name, "You're not channel operator");

    if (argument.size() < 2) // SET TOPIC
        return ERR(client, 461, "TOPIC", "Not enough parameters");

    if (argument[1][0] == ':') // ENLEVE LE PREFIX
        argument[1] = argument[1].substr(1);

    size_t tmp = new_topic.find(':');
    if (tmp != std::string::npos)
        msg = new_topic.substr(tmp + 1);
    else
        msg = " ";
    channel->SET_Topic(msg); // SET TOPIC mais seulement avec le 1er elements et non toute la phrase

    msg = ":" + client->get_Prefix() + " TOPIC " + channel_name + " :" + channel->GET_Topic(); // pas sur du msg, client ne recois rien comme quoi topic a ete change
    channel->Send_Msg_To_All_Client(msg);
}

void Server::KICK(Client *client, std::vector<std::string> arguments, std::string msg)
{
    std::cout << "KICK DETECTED" << std::endl;
    
    std::cout << "avant :" << msg << std::endl;

    size_t tmp = msg.find(':');
    if (tmp != std::string::npos)
    {
        msg = msg.substr(tmp + 1);
        if (msg.empty())
            msg.clear();
    }
    else
        msg.clear();

    std::cout << "APRES " << msg << std::endl;

    if (arguments.size() < 2)
        return ERR(client, 461, "Kick", "Not enough parameters");
    
    std::string channel_name = arguments[0];
    Channel *channel = CHANNEL_Exist(channel_name);
    if (!channel)
        return ERR(client, 403, channel_name, "No such channel");
    
    if (channel->Is_Operator(client) == 0) // CHECK IF CLIENT IS OPERATOR
        return ERR(client, 482, client->get_nick() + " " + channel_name, "You're not channel operator");
    
    if (channel->Client_in_Channel(client->get_nick()) == 0) // CHECK IF CLIENT IS IN THE CHANNEL
        return ERR(client, 442, channel_name, "You're not on that channel");
    
    if (channel->Client_in_Channel(arguments[1]) == 0) // CHECK IF CLIENT IS IN THE CHANNEL
        return ERR(client, 441, arguments[1] + " " + channel_name, "They aren't on that channel");
    if (msg.empty())
        msg = "Kick";
    
    channel->Send_Msg_To_All_Client(":" + client->get_Prefix() + " KICK " + channel->GET_Name() + " " + arguments[1] + " :" + msg);
    channel->DELETE_User(FINDING_Client_str(arguments[1]));
    //channel->Send_Msg_To_All_Client(": " + client->get_Prefix() + " KICK " + channel->GET_Name() + " " + arguments[1] + " :" + msg);
    //channel->SEND_Msg_to_everyone("Kick " + arguments[1] + " From " + channel_name + " using " + msg + " as the reason.", client);
}

void Server::PART(Client *client, std::vector<std::string>argument, std::string message)
{
    std::cout << "PART DETECTED" << std::endl;

    if (argument.size() < 1)
        return ERR(client, 461, "PART", "Not enough parameters");
    
    Channel     *channel;
    std::string channel_name;
    std::vector<std::string> Names = split(argument[0], ',');
    std::string keys;

    size_t tmp = message.find(':');
    if (tmp != std::string::npos)
    {
        keys = message.substr(tmp + 1);
        if (keys.empty())
            keys = "Leaving";
    }
    else
        keys = "Leaving";

    std::cout << "keys : " << keys << std::endl;
    

    for (size_t i = 0; i < Names.size(); ++i)
    {
        channel_name = Names[i];
        channel = CHANNEL_Exist(channel_name);
        if (!channel)
            return ERR(client, 403, channel_name, "No such channel");
        if (channel->Client_in_Channel(client->get_nick()) == 0)
            return ERR(client, 442, channel_name, "You're not on that channel");
        
        channel->Send_Msg_To_All_Client(":" + client->get_Prefix() + " PART " + channel_name + " :" + keys);
        channel->DELETE_User(client);
    }

    if (channel->GET_Nb_User() == 0) // Si le channel est vide on le supprime
    {
        for (size_t i = 0; i < _channels.size(); ++i)
        {
            if (_channels[i]->GET_Name() == channel->GET_Name())
            {
                _channels.erase(_channels.begin() + i);
                delete channel;
                break;
            }
        }
    }
}
