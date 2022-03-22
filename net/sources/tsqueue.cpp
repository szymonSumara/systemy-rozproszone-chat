////
//// Created by szymon on 02.03.2022.
////

#include "../headers/tsqueue.h"
#include "../net.h"
#include <mutex>

template <typename type>
type TSQueue<type>::get(){
    std::unique_lock<std::mutex> lk(queueMutex);
    while(data.empty()) {
        queueCondition.wait(lk);
        if(isClosed) throw std::string ("Queue is closed");
    }
    type frontValue = data.front();
    data.pop_front();
    return frontValue;
}

template <typename type>
void TSQueue<type>::put(type item){
    std::unique_lock<std::mutex> lk(queueMutex);
    data.push_back(item);
    queueCondition.notify_one();
}

template <typename type>
void TSQueue<type>::close(){
    isClosed = true;
    queueCondition.notify_all();
}

template class TSQueue<Message>;