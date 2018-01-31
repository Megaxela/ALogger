#set()
set(CMAKE_CXX_STANDARD 17)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/googletest/CMakeLists.txt")
    add_subdirectory(googletest)
endif()

add_executable(ALoggerTest
        main.cpp
)

target_include_directories(ALoggerTest PRIVATE
        googletest/googletest/include
)

target_link_libraries(ALoggerTest
        gtest
        ALogger
)