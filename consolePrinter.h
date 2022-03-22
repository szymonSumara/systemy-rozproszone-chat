//
// Created by szymon on 10.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE_CONSOLEPRINTER_H
#define SYSTEMY_ROZPROSZONE_CONSOLEPRINTER_H

#include <mutex>
#include <condition_variable>
#include <list>
#include <exception>
#include <iostream>
#define LINE_WIDTH 20

class consolePrinterProxy{
public:
    std::mutex consoleLock;
    std::string linePrefix;
    std::string actualLine;

    void printLineAbove(std::string text){
        std::unique_lock<std::mutex> lk(consoleLock);
        clearLine();
        std::cout << text << "\n\r" << linePrefix << actualLine ;
        fflush(stdout);
    }

private:
    void clearLine(){
        std::cout << "\r";
        for(int i = 0; i < actualLine.size() + linePrefix.size() + 20; i++){
            std::cout << " ";
            //std::cout << "\n";
        }
        std::cout << "\r" ;//<< linePrefix;
      //  actualLine = "";
    }
public:

    void clearInput(){
        std::unique_lock<std::mutex> lk(consoleLock);
        actualLine = "";
        std::cout << "\r" << linePrefix ;
    }

    void putChar(char character){
        std::unique_lock<std::mutex> lk(consoleLock);
        actualLine += character;
        clearLine();
        std::cout << "\r" << linePrefix <<   actualLine;
    }

    void removeChar(){
        std::unique_lock<std::mutex> lk(consoleLock);
        if( actualLine.size() != 0 ) actualLine.pop_back();
        clearLine();
        std::cout << "\r" << linePrefix <<   actualLine;
    }

    void setPrefix(std::string prefix){
        std::unique_lock<std::mutex> lk(consoleLock);
        clearLine();
        linePrefix = prefix;
        std::cout << "\r" << linePrefix << actualLine;
    }


//    consolePrinterProxy& lockConsole(std::string text){
//        consoleLock.lock();
//    }
//
//    consolePrinterProxy& clearLine(){
//        std::cout << "\r";
//        for(int i = 0; i < LINE_WIDTH; i++) std::cout << " ";
//        std::cout << "\r";
//    }
//
//    consolePrinterProxy& write(std::string text){
//        std::cout << "\r" + text;
//        return *this;
//    }
//
//    consolePrinterProxy& endLine(){
//        std::cout << "\n";
//        return *this;
//    }
//
//    consolePrinterProxy& realiseConsole(std::string text){
//        consoleLock.unlock();
//    }



};

#endif //SYSTEMY_ROZPROSZONE_CONSOLEPRINTER_H
