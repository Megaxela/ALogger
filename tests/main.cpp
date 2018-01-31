
#include <Loggers/BasicLogger.hpp>
#include <Stream.hpp>
#include "gtest/gtest.h"
#define DebugF(L)    Loggers::Stream(L, AbstractLogger::ErrorClass::Debug,   __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)
#define InfoF(L)     Loggers::Stream(L, AbstractLogger::ErrorClass::Info,    __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)
#define WarningF(L)  Loggers::Stream(L, AbstractLogger::ErrorClass::Warning, __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)
#define ErrorF(L)    Loggers::Stream(L, AbstractLogger::ErrorClass::Error,   __FILENAME__, __LINE__, std::this_thread::get_id(), std::string(), __FUNCTION__)


TEST(ALogger, SetFormat)
{
    auto logger = std::make_shared<Loggers::BasicLogger>();

    InfoF(logger) << "Example output";
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}