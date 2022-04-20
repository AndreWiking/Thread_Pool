cmake_minimum_required(VERSION 3.21)
project(Thread_Pool)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_OSX_ARCHITECTURES "x86_64;" CACHE STRING "Build architectures for Mac OS X" FORCE)

add_executable(Thread_Pool Source/main.cpp Source/ThreadPool.cpp Source/ThreadPool.h Source/UnboundedBlockingQueue.cpp Source/UnboundedBlockingQueue.h)
