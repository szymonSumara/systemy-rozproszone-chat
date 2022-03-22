//
// Created by szymon on 02.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE_CLIENT_H
#define SYSTEMY_ROZPROSZONE_CLIENT_H

#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <list>
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include "connection.h"
#include "../../consolePrinter.h"

class Client {
    Connection * connection;
    TSQueue<Message> messageQueue;
    std::thread messagePrinter;
    std::thread UDPRoutineThread;
    std::thread UDPMulticastRoutineThread;
    int clientSocketTCP;
    int clientSocketUDP;
    int clientSocketMulticast;
    struct sockaddr_in groupSock;
    struct sockaddr_in     serverAdress;
    consolePrinterProxy& console;

public:
    std::string nick;


    Client(char * addres, int port, consolePrinterProxy & console ) : console(console){
        createSockets();
        connect(addres,port);
        setUpMulticast();

        connection = new Connection(clientSocketTCP, messageQueue );
        connection->run();
    }

    void connect(char * address, int port );

    void run(){
        messagePrinter = std::thread(&Client::printMessageLoop,this);
        UDPRoutineThread = std::thread(&Client::UDPRoutine,this);
        UDPMulticastRoutineThread = std::thread(&Client::UDPMulticastRoutine,this);
    };

    void printMessageLoop();



public:
    void setUpMulticast();
    void send(Message message);
    void UDPRoutine();

    void UDPMulticastRoutine();

    void sendUDPMessage(Message message);
    void sendMulticastMessage(Message message);

    void createSockets();

    ~Client();
};


#endif //SYSTEMY_ROZPROSZONE_CLIENT_H
