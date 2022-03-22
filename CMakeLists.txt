cmake_minimum_required(VERSION 3.20)
project(systemy_rozproszone)

set(CMAKE_CXX_STANDARD 14)
set(Sources net/sources/tsqueue.cpp net/sources/connection.cpp  net/sources/client.cpp net/sources/server.cpp net/net.h consolePrinter.h inputCollector.h )
set(Headers net/headers/tsqueue.h net/headers/connection.h  net/headers/client.h net/headers/server.h )

add_executable(server server.cpp
        ${Sources} ${Headers}
        )

add_executable(client client.cpp   ${Sources} ${Headers})


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)