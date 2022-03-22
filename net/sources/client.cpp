//
// Created by szymon on 02.03.2022.
//

#include "../headers/client.h"


void Client::connect(char * address, int port ) {

    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(port);

    if (inet_pton(AF_INET, address, &serverAdress.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(-1);
    }
    int c = ::connect(clientSocketTCP, (struct sockaddr *) &serverAdress, sizeof(serverAdress));
    if (c < 0) {
        printf("\nConnection Failed \n");
        exit(-1);
    }

    {
        // enable SO_REUSEADDR to allow multiple instances of this application to
        //    receive copies of the multicast datagrams.
        int reuse = 1;

        if (setsockopt(clientSocketMulticast, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0) {
            perror("SO_REUSEADDR");
            exit(1);
        }
    }

}

void Client::setUpMulticast() {
    sockaddr_in localSock = {};    // initialize to all zeroes
    localSock.sin_family      = AF_INET;
    localSock.sin_port        = htons(4321);
    localSock.sin_addr.s_addr = INADDR_ANY;

    groupSock = {};   // init to all zeroes
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
    groupSock.sin_port = htons(4321);

    {
        char loopch = 0;
        if(setsockopt(clientSocketMulticast, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0){
            perror("Setting IP_MULTICAST_LOOP error");
            close(clientSocketMulticast);
            exit(1);
        }
    }


    if( bind(clientSocketMulticast, (sockaddr*)&localSock, sizeof(localSock)) < 0){
        perror("bind");
        exit(1);
    }


    ip_mreq group = {};    // initialize to all zeroes
    group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
    group.imr_interface.s_addr = inet_addr("127.0.0.1");

    if( setsockopt(clientSocketMulticast, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&group,
                   sizeof(group)) < 0)
    {
        perror("add to group");
        exit(1);
    }

    in_addr localIface = {};   // init to all zeroes
    localIface.s_addr = inet_addr("127.0.0.1");



    if( setsockopt(clientSocketMulticast, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localIface,
                   sizeof(localIface)) < 0)
    {
        perror("add to group");
        exit(1);
    }
}

void Client::printMessageLoop(){
    try{
        while(true){
            Message m = messageQueue.get();
            if(m.type == MessageTypes::DATA || m.type == MessageTypes::MULTICAST_DATA)
                console.printLineAbove( std::string() + "[ " + m.nick + " ]: \n" + m.message );
            if(m.type == MessageTypes::TEXT)
                console.printLineAbove( std::string() + "[ " + m.nick + " ]: " + m.message );
        }
    }catch( std::string  exeption){
        std::cout << exeption;
    }

}

void Client::sendUDPMessage(Message message){
    char buffer[sizeof (message)];
    std::memcpy(&buffer, &message, sizeof(message));
    sendto(clientSocketUDP, buffer, sizeof (message),
           MSG_CONFIRM, (const struct sockaddr *) &serverAdress,
           sizeof(serverAdress));
}

void Client::sendMulticastMessage(Message message){
    char buffer[sizeof (message)];
    std::memcpy(&buffer, &message, sizeof(message));
    sendto(clientSocketMulticast, buffer, sizeof(message), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
}

void Client::send(Message message){


    if(message.type == MessageTypes::TEXT){
        if(std::string(message.message) != "INIT")
            console.printLineAbove( std::string() + "[ me ] : " + message.message );
        if(connection->isConnected) connection->send(message);
        else console.printLineAbove("-- server is disconnected --");
    }
    else if(message.type == MessageTypes::DATA){
        if(std::string(message.message) != "INIT")
            console.printLineAbove( std::string() + "[ me ] : \n" + message.message );
        sendUDPMessage(message);

    }else{
        console.printLineAbove( std::string() + "[ me ] : \n" + message.message );
        sendMulticastMessage(message);
    }
}

void Client::UDPRoutine(){

    char buffer[sizeof (Message)];
    while(true){
        int n =  recv(clientSocketUDP, buffer, sizeof (Message),0);
        if( n <= 0 ) break;
        Message m;
        std::memcpy(&m, buffer , sizeof(m));
        messageQueue.put(m);
    }
}

void Client::UDPMulticastRoutine(){

    char buffer[sizeof (Message)];

    while(true){
        int n =  recv(clientSocketMulticast, buffer, sizeof (Message),0);
        if( n <= 0 ) break;
        Message m;
        std::memcpy(&m, buffer , sizeof(m));
        if(std::string (m.nick) == nick) continue;
        messageQueue.put(m);
    }
}

void Client::createSockets(){
    if((clientSocketTCP = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(-1);
    }

    if((clientSocketUDP = ::socket(AF_INET,SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(-1);
    }

    if((clientSocketMulticast = ::socket(AF_INET,SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(-1);
    }
}

Client::~Client(){

    shutdown(clientSocketMulticast, SHUT_RDWR);
    shutdown(clientSocketUDP, SHUT_RDWR);
    shutdown(clientSocketTCP, SHUT_RDWR);

    messageQueue.close();
    messagePrinter.join();
    UDPRoutineThread.join();
    UDPMulticastRoutineThread.join();

    delete connection;

}