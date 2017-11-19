cmake_minimum_required(VERSION 3.5)
project(ALogger)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

if (WIN32)
    add_definitions(-DOS_WINDOWS)
else()
    add_definitions(-DOS_LINUX)
endif()

set(SOURCE_FILES
        src/Loggers/AbstractLogger.cpp
        src/Loggers/BasicLogger.cpp
        src/SystemTools.cpp
        src/CurrentLogger.cpp
        src/Stream.cpp
)

set(INCLUDE_FILES
        include/Loggers/AbstractLogger.hpp
        include/Loggers/BasicLogger.hpp
        include/SystemTools.h
        include/CurrentLogger.hpp
        include/Stream.hpp
)

add_library(ALogger
        ${SOURCE_FILES}
        ${INCLUDE_FILES}
)