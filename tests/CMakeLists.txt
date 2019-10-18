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

if (EMSCRIPTEN)
    # Cause we need page, instead of plain js
    set_target_properties(ALoggerTest
        PROPERTIES
            SUFFIX ".html"
    )

    # Application WILL be closed
    target_link_libraries(ALoggerTest
        "-s\ EXIT_RUNTIME=1"
    )
endif()