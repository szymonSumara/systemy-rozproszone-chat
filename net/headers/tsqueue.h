//
// Created by szymon on 02.03.2022.
//

#ifndef SYSTEMY_ROZPROSZONE_TSQUEUE_H
#define SYSTEMY_ROZPROSZONE_TSQUEUE_H


#include <mutex>
#include <condition_variable>
#include <list>
#include <exception>

template <typename type>
class TSQueue {

    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::list<type> data;
    volatile bool isClosed = false;

public:

    type get();
    void put(type item);
    void close();

};

#endif //SYSTEMY_ROZPROSZONE_TSQUEUE_H



