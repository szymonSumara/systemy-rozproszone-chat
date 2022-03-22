//
// Created by szymon on 11.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE_INPUTCOLLECTOR_H
#define SYSTEMY_ROZPROSZONE_INPUTCOLLECTOR_H
#include <iostream>
#include "consolePrinter.h"

class InputCollector{
public:
    consolePrinterProxy& console;

    InputCollector(consolePrinterProxy& console) : console(console){
        //-ignbrk -brkint -ignpar -parmrk -iuclc -ixany -imaxbel -xcase -inpck  -istrip -ixon  -ixoff -opost
       system("stty  -inlcr -igncr  -icrnl   -icanon   min 1 time 0");
    };

    std::string getLine(){
        //std::cout << "getLine";
        std::string input;
        char c = ' ';

        c = getchar();

        while((int)c != 13){

           // std::cout << "inside" << (int)c ;
            if((int)c == 127) {
                if( input.size() != 0 )input.pop_back();
                console.removeChar();

            }else{
                console.putChar(c);
                input += c;
            }

            c = getchar();

           // std::cout << (int)c << "\n";
        }
        console.clearInput();
        //std::cout << "exit";
        return input;
    }
};
#endif //SYSTEMY_ROZPROSZONE_INPUTCOLLECTOR_H
