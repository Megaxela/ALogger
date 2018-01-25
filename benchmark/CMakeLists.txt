
set(CMAKE_CXX_STANDARD 17)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/benchmark/CMakeLists.txt")
    add_subdirectory(benchmark)
endif()

add_executable(ALoggerBenchmark
        main.cpp
        IostreamsLock.cpp
        IostreamsLock.hpp
        Utilities.hpp
)

target_link_libraries(ALoggerBenchmark
        ALogger
        benchmark
)