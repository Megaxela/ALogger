cmake_minimum_required(VERSION 3.9)
project(ALogger)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

if (WIN32)
    add_definitions(-DOS_WINDOWS)
else()
    add_definitions(-DOS_LINUX)
endif()

add_executable(ALogger
        main.cpp
        src/Loggers/AbstractLogger.cpp
        include/Loggers/AbstractLogger.hpp
        src/Loggers/BasicLogger.cpp
        include/Loggers/BasicLogger.hpp
        src/SystemTools.cpp
        include/SystemTools.h src/CurrentLogger.cpp include/CurrentLogger.hpp src/Stream.cpp include/Stream.hpp)