//
// Created by szymon on 02.03.2022.
//

#include "../headers/connection.h"
#include <iostream>


void Connection::messageCollectRoutine(){
    char buffer[sizeof(Message)];
    while(isConnected){
        int receivedBytes = recv(connectionSocketTCP, buffer, sizeof(Message), 0);
        if(receivedBytes <= 0){
            if(receivedBytes == 0){
                this->isConnected = false;
            }else{
                perror("recv");
                exit(EXIT_FAILURE);
            }
        }else if(receivedBytes != 0){
            Message m;
            std::memcpy(&m, buffer, sizeof(m));
            nick = std::string(m.nick);
            messageQueue.put(m);
        }
    }
}


void Connection::send(Message message){
    char buffer[sizeof(Message)];
    std::memcpy(buffer, &message, sizeof(Message));
    int res = ::send(connectionSocketTCP, buffer, sizeof(Message), 0);
}