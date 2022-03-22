//
// Created by szymon on 08.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE1_PICTURES_H
#define SYSTEMY_ROZPROSZONE1_PICTURES_H
#include "net/net.h"
#include <fstream>


int readPictureFromFile(char * buffer, std::string imagePath){

        int bufferPosition =0 ;
        char c = 0;

        std::ifstream input_file("images/" + imagePath);
        if (!input_file.is_open()) {
            input_file.close();
            return -1;
        }

        while (input_file.get(c) && bufferPosition != (MAX_MESSAGE_SIZE - 1)) {
            buffer[bufferPosition] = c;
            bufferPosition+=1;
        }

        buffer[bufferPosition] = '\0';

        return 0;

}

#endif //SYSTEMY_ROZPROSZONE1_PICTURES_H
