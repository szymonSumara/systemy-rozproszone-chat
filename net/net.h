//
// Created by szymon on 02.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE_NET_H
#define SYSTEMY_ROZPROSZONE_NET_H

const int MAX_MESSAGE_SIZE =  2048;
const int MAX_NICK_LENGTH = 64;

enum MessageTypes{
    TEXT,
    DATA,
    MULTICAST_DATA,
};

struct Message{
    char nick[MAX_NICK_LENGTH];
    MessageTypes type;
    char ipAddress[20];
    int port;
    char message[MAX_MESSAGE_SIZE];
};

#include "headers/connection.h"
#include "headers/tsqueue.h"






#endif //SYSTEMY_ROZPROSZONE_NET_H
