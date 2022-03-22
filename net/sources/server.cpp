//
// Created by szymon on 02.03.2022.
//

#include "../headers/server.h"

void Server::createSocket(){
    serverSocketTCP = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketTCP == 0 )
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serverSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocketUDP == 0 )
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int on = 1;
    if (setsockopt(serverSocketTCP, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
};



void Server::bindToPort( int port){
    struct sockaddr_in address;
    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    if (bind(serverSocketTCP, (struct sockaddr *)&address,sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if ( bind(serverSocketUDP, ( struct sockaddr *)&address, sizeof(address)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "[ SERVER ] : Bind socket to port "<< port << "\n";
};

void Server::listen(){
    if (::listen(serverSocketTCP, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "[ SERVER ] : Start Listen\n";

};

void Server::start(){
    broadcaster = std::thread(&Server::messageRoutine, this);
    udpMessageCollector = std::thread(&Server::UDPRoutine, this);
    serverRoutine();
}

void Server::serverRoutine(){
    while(true){
        int newConnectionSocket = accept(serverSocketTCP, nullptr, nullptr);
        if(newConnectionSocket < 0){
            perror("accept");
            exit(EXIT_FAILURE);
        }

        connections.push_back( new Connection(newConnectionSocket, messageQueue));
        connections.back()->run();

        Message m;
        m.type = MessageTypes::TEXT;
        sprintf(m.nick, "Server");
        sprintf(m.message, "Welcome in chatroom");

        connections.back()->send(m);
    }
}

void Server::UDPRoutine(){
    char buffer[sizeof (Message)];
    struct sockaddr_in address;

    while(true){

        socklen_t len = sizeof (address);
        int n = recvfrom(serverSocketUDP, (char *)buffer, sizeof (Message),
                         MSG_WAITALL, ( struct sockaddr *) &address,
                         &len);
        if(n <= 0) break;

        Message m;
        std::memcpy(&m , buffer, sizeof(m));
        sprintf(m.ipAddress, "%s",inet_ntoa(address.sin_addr));
        m.port = htons(address.sin_port);

        messageQueue.put(m);
    }
}

void Server::messageRoutine(){
    try{
        while(true){
            Message m = messageQueue.get();
            filterConnections();
            if(m.type == MessageTypes::DATA){
                if(std::string(m.message) == "INIT"){
                    updateSenderAddress(m);
                    continue;
                }
                std::cout << "[ " << m.nick << " ] : \n" << m.message << std::endl;
                UDPboradcast(m);
            }else{
                if(std::string(m.message) == "INIT"){
                    sprintf(m.message, "%s",(std::string(m.nick) + " connected to chat").c_str());
                    sprintf(m.nick, "%s","Server");
                    broadcast(m);
                    continue;
                }
                std::cout << "[ " << m.nick << " ] : " << m.message << std::endl;
                broadcast(m);
            }
        }
    }catch( std::string  exeption){
        std::cout << exeption;
    }

}


void Server::updateSenderAddress(Message m){
    for(Connection * c : connections){
        if(c->nick == std::string(m.nick)) c->setAddress(m.ipAddress,m.port);
    }
}

void Server::filterConnections(){
    connections.remove_if([this](Connection * val)
                          {
                              if(!val->isConnected){
                                  delete val;


                                  Message m;
                                  m.type = MessageTypes::TEXT;
                                  sprintf(m.nick, "Server");
                                  sprintf(m.message, "%s",(char *)(std::string (val->nick) + " is disconnect").c_str());
                                  broadcast(m);

                                  return  true;
                              }

                              return false;
                          });

    std::cout << "[ SERVER ]: Alive connection after filter: " << connections.size() << "\n";
}

void Server::clearConnections(){
    connections.remove_if([](Connection * val)
                          {
                              delete val;
                              return true;
                          });

    std::cout << "[ SERVER ]: Active connections: " << connections.size() << "\n";
}

void Server::broadcast(struct Message m){
    std::string nick(m.nick);
    for(Connection * c : connections){
        if(c->nick != nick && c->isConnected)
            c->send(m);
    }
}

void Server::UDPboradcast(Message m){
    char buffer[sizeof(m)];
    std::memcpy(buffer,&m, sizeof (Message));
    for(Connection * c : connections){
        if(c->nick != std::string(m.nick)  && c->isConnected) {
            int n = sendto(serverSocketUDP, buffer, sizeof(Message),
                           MSG_WAITALL, (struct sockaddr *) c->address,
                           sizeof(struct sockaddr_in));
        }
    }
}

Server::~Server(){

    shutdown(serverSocketTCP, SHUT_RDWR);
    shutdown(serverSocketUDP, SHUT_RDWR);

    messageQueue.close();
    broadcaster.join();
    udpMessageCollector.join();
    clearConnections();

}