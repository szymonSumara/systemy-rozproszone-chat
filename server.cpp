#include <iostream>
#include "net/headers/server.h"
#include <csignal>

using namespace std;

Server * server;

void signalHandler( int signum ) {
    cout << "Interrupt signal (" << signum << ") received.\n";
    flush(cout);
    delete server;
    exit(signum);
}

int main(){
    signal(SIGINT, signalHandler);
    server = new Server(3001);
    server->start();
    std::cout << "Hello world";
    delete server;
};
