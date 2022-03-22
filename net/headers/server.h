//
// Created by szymon on 02.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE_SERVER_H
#define SYSTEMY_ROZPROSZONE_SERVER_H


#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <list>
#include <iostream>
#include <thread>

#include "connection.h"

class Server{

    int serverSocketTCP;
    int serverSocketUDP;

    std::list<Connection *> connections;
    std::thread broadcaster;
    std::thread udpMessageCollector;

    TSQueue<Message> messageQueue;
    bool shouldStop;

public:
    Server(int port){
        createSocket();
        bindToPort(port);
        listen();
    }

    void createSocket();
    void bindToPort( int port);
    void listen();
    void start();
    void serverRoutine();
    void UDPRoutine();
    void messageRoutine();
    void updateSenderAddress(Message m);
    void filterConnections();
    void clearConnections();
    void broadcast(struct Message m);
    void UDPboradcast(Message m);
    ~Server();
};


#endif //SYSTEMY_ROZPROSZONE_SERVER_H
