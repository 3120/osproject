cmake_minimum_required(VERSION 3.6)
project(osproj C CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        node.c
        node.h
        queue.c
        queue.h
        rcb.c
        rcb.h
        rr.c
        rr.h
        sjf.c
        sjf.h
        sws.c
        network.c
        network.h
        http.c
        http.h
        sws.h
        globals.h
        mlfb.h
        mlfb.c)

find_package(Threads REQUIRED)
add_executable(osproj ${SOURCE_FILES})
set(PThreadLib -pthread)
target_link_libraries(osproj ${PThreadLib})