#include <benchmark/benchmark.h>
#include <Loggers/AbstractLogger.hpp>
#include <memory>
#include <Stream.hpp>
#include <cstring>
#include <Loggers/BasicLogger.hpp>
#include <Loggers/AsyncLogger.hpp>
#include <iostream>
#include "IostreamsLock.hpp"
#include "Utilities.hpp"

#define DebugF(L)    Loggers::Stream(L, AbstractLogger::ErrorClass::Debug,   __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)
#define InfoF(L)     Loggers::Stream(L, AbstractLogger::ErrorClass::Info,    __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)
#define WarningF(L)  Loggers::Stream(L, AbstractLogger::ErrorClass::Warning, __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)
#define ErrorF(L)    Loggers::Stream(L, AbstractLogger::ErrorClass::Error,   __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)

#define TEST_LOG_STRING "EXAMPLE_LOG_STRING"

class DummyLogger : public AbstractLogger
{
public:
protected:
    void onNewMessage(const Message& message) override
    {
    }
};

template<typename T>
static void normalWithoutLogFileFunctionLogging(benchmark::State& state)
{
    IostreamsLock lock;

    auto logger = std::make_shared<T>();

    // Removing log folder if it's exists
    deleteFolder(logger->logPath());

    for (auto _ : state)
    {
        for (int i = 0; i < state.range(0); ++i)
        {
            InfoF(logger) << TEST_LOG_STRING;
        }
    }

    state.SetComplexityN(state.range(0));
    logger->waitForLogToBeWritten();
}

template<typename T>
static void normalWithLogFileFunctionLogging(benchmark::State& state)
{
    IostreamsLock lock;

    auto logger = std::make_shared<T>();

    // Creating log folder if it doesn't exists
    createPath(logger->logPath());

    for (auto _ : state)
    {
        for (int i = 0; i < state.range(0); ++i)
        {
            InfoF(logger) << TEST_LOG_STRING;
        }
    }

    state.SetComplexityN(state.range(0));
    logger->waitForLogToBeWritten();
}

template<typename T>
static void suppressedFunctionLogging(benchmark::State& state)
{
    IostreamsLock lock;

    auto logger = std::make_shared<T>();

    logger->setMinimumTerminalOutputErrorClass(T::ErrorClass::None);
    logger->setMinimumFileOutputErrorClass(T::ErrorClass::None);

    for (auto _ : state)
    {
        for (int i = 0; i < state.range(0); ++i)
        {
            InfoF(logger) << TEST_LOG_STRING;
        }
    }

    state.SetComplexityN(state.range(0));
    logger->waitForLogToBeWritten();
}

constexpr int RANGE_START = 1;
constexpr int RANGE_END = 1 << 15;

BENCHMARK_TEMPLATE(normalWithLogFileFunctionLogging,    Loggers::BasicLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();
BENCHMARK_TEMPLATE(normalWithoutLogFileFunctionLogging, Loggers::BasicLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();
BENCHMARK_TEMPLATE(suppressedFunctionLogging,           Loggers::BasicLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();

BENCHMARK_TEMPLATE(normalWithLogFileFunctionLogging,    Loggers::AsyncLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();
BENCHMARK_TEMPLATE(normalWithoutLogFileFunctionLogging, Loggers::AsyncLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();
BENCHMARK_TEMPLATE(suppressedFunctionLogging,           Loggers::AsyncLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();

BENCHMARK_TEMPLATE(normalWithLogFileFunctionLogging,    DummyLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();
BENCHMARK_TEMPLATE(normalWithoutLogFileFunctionLogging, DummyLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();
BENCHMARK_TEMPLATE(suppressedFunctionLogging,           DummyLogger)
    ->Range(RANGE_START, RANGE_END)
    ->Complexity();

BENCHMARK_MAIN();
