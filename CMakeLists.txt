cmake_minimum_required(VERSION 3.5)
project(ALogger)


option(ALOGGER_BUILD_TESTS "Build tests for logger" Off)
option(ALOGGER_BUILD_BENCHMARK "Build benchark for logger" Off)
option(ALOGGER_BUILD_ASYNC_LOGGER "Build async logger" On)

set(CMAKE_CXX_STANDARD 17)

set(MAIN_SOURCE_FILES
    src/Loggers/AbstractLogger.cpp
    src/Loggers/BasicLogger.cpp
    src/SystemTools.cpp
    src/CurrentLogger.cpp
    src/Stream.cpp
)

set(ASYNC_SOURCE_FILES
    src/Loggers/AsyncLogger.cpp
)

set(SOURCE_FILES
    ${MAIN_SOURCE_FILES})

if (${ALOGGER_BUILD_ASYNC_LOGGER})
    set(SOURCE_FILES ${SOURCE_FILES} ${ASYNC_SOURCE_FILES})
endif()

add_library(ALogger
    ${SOURCE_FILES}
)

# Currently required only for errors and escaping
if (WIN32)
    target_compile_definitions(ALogger PRIVATE
        -DOS_WINDOWS
    )
else()
    target_compile_definitions(ALogger PRIVATE
        -DOS_LINUX
    )
endif()

if (EMSCRIPTEN)
    message(STATUS "Detected emscripten compiler. Adding USE_PTHREADS if required")

    if (${ALOGGER_BUILD_ASYNC_LOGGER})
        # See https://gitlab.kitware.com/cmake/cmake/issues/15826 for discussion
        target_compile_options(ALogger PRIVATE
                SHELL:-s\ USE_PTHREADS=1
                SHELL:-s\ PROXY_TO_PTHREAD=1
                SHELL:-s\ EXIT_RUNTIME=1
        )

        # Workaround, cause target_link_options does not work
        target_link_libraries(ALogger PRIVATE
                "-s\ USE_PTHREADS=1"
                "-s\ PROXY_TO_PTHREAD=1"
        )
    endif()
endif()

target_include_directories(ALogger PUBLIC
    include
)

# Tests And Benchmark
if (${ALOGGER_BUILD_TESTS})
    add_subdirectory(tests)
endif()

if (${ALOGGER_BUILD_BENCHMARK})
    add_subdirectory(benchmark)
endif()