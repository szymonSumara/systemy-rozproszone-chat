//
// Created by szymon on 02.03.2022.
//
#include <iostream>
#include<string.h>
#include "net/headers/client.h"
#include "./pictures.h"
#include <csignal>
#include "consolePrinter.h"
#include "inputCollector.h"

Client  * client;

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    flush(std::cout);
    delete client;
    exit(signum);
}

int main(){
    consolePrinterProxy console;
    InputCollector inputCollector(console);

    std::string nick;
    console.setPrefix("Enter your nick: ");
    nick = inputCollector.getLine();
    console.printLineAbove(std::string ("Hello ") + nick);
    console.setPrefix(std::string() + "[ " + nick + " ] : ");
    Message m;
    std::memcpy(m.nick , nick.c_str(), nick.size());
    m.nick[nick.size()] ='\0';

    client = new Client((char *)"127.0.0.1",3001, console);
    client->nick = nick;
    signal(SIGINT, signalHandler);

    client->run();
    std::string text("INIT");
    std::memcpy( m.message, text.c_str(), text.size());
    m.message[text.size()] =  '\0';
    m.type = MessageTypes::TEXT;
    client->send(m);
    sleep(1);
    m.type = MessageTypes::DATA;
    client->send(m);

    while(true){

        memset(m.message, 0, MAX_MESSAGE_SIZE);
        text = inputCollector.getLine();

        if(text == "U"){
            m.type = MessageTypes::DATA;
            console.printLineAbove("-- Change protocol to UDP --");
            continue;
        }

        if(text == "T"){
            m.type = MessageTypes::TEXT;
            console.printLineAbove("-- Change protocol to TCP --");
            continue;
        }

        if(text == "M"){
            m.type = MessageTypes::MULTICAST_DATA;
            console.printLineAbove("-- Change to Multicast --");
            continue;
        }

        if( m.type == MessageTypes::DATA ||  m.type == MessageTypes::MULTICAST_DATA){
            if(readPictureFromFile(m.message, text) < 0){
                console.printLineAbove("-- File " + text + " not found in images directory");
                continue;
            }
        }else{
            std::memcpy( m.message, text.c_str(), text.size());
        }

        client->send(m);
    }
};
