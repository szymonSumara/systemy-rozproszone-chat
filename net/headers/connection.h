//
// Created by szymon on 02.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE_CONNECTION_H
#define SYSTEMY_ROZPROSZONE_CONNECTION_H

#include <thread>
#include <netinet/in.h> //send, read
#include "tsqueue.h"
#include <cstring>
#include "../net.h"
#include <iostream>
#include <arpa/inet.h>

class Connection{

    bool isCopy;

    TSQueue<Message> &messageQueue;

public:
    int connectionSocketTCP;

    struct sockaddr_in  * address;

    bool isConnected;
    std::thread reciveMessageRoutine;
    std::string  nick;

    Connection(int socketFd, TSQueue<Message> &messageQueue) :nick(""), messageQueue(messageQueue){
        connectionSocketTCP = socketFd;
        address = new sockaddr_in();
        isCopy = false;
    }

    Connection(Connection const &connection) :  messageQueue(connection.messageQueue) {
        connectionSocketTCP = connection.connectionSocketTCP;
        nick = connection.nick;
        address = connection.address;
        isConnected = connection.isConnected;
        isCopy = true;

    }

public:
    void setAddress(char * adr, int port){

        address->sin_family = AF_INET;
        address->sin_port = htons(port);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if(inet_pton(AF_INET,        adr, &(address->sin_addr))<=0)
        {
            printf("\nInvalid address/ Address not supported \n");
            exit(-1);
        }
    }

    void send(Message message);
    void messageCollectRoutine();

    void run(){
        isConnected = true;
        reciveMessageRoutine = std::thread(&Connection::messageCollectRoutine,this);
    }


    ~Connection(){
        if(!isCopy) shutdown(connectionSocketTCP, SHUT_RDWR);
        if( reciveMessageRoutine.joinable()) reciveMessageRoutine.join();
        if(!isCopy) free(address);
    }
};

#endif //SYSTEMY_ROZPROSZONE_CONNECTION_H


