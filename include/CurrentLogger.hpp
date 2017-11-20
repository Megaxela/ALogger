//
// Created by megaxela on 11/8/17.
//

#pragma once

#include "Loggers/AbstractLogger.hpp"
#include <Stream.hpp>
#include <cstring>
#include <SystemTools.h>

#define Debug()    Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Debug,   __FILENAME__, __LINE__, SystemTools::getTypeName(*this), __FUNCTION__)
#define Info()     Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Info,    __FILENAME__, __LINE__, SystemTools::getTypeName(*this), __FUNCTION__)
#define Warning()  Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Warning, __FILENAME__, __LINE__, SystemTools::getTypeName(*this), __FUNCTION__)
#define Error()    Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Error,   __FILENAME__, __LINE__, SystemTools::getTypeName(*this), __FUNCTION__)

#define DebugF()    Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Debug,   __FILENAME__, __LINE__, std::string(), __FUNCTION__)
#define InfoF()     Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Info,    __FILENAME__, __LINE__, std::string(), __FUNCTION__)
#define WarningF()  Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Warning, __FILENAME__, __LINE__, std::string(), __FUNCTION__)
#define ErrorF()    Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Error,   __FILENAME__, __LINE__, std::string(), __FUNCTION__)

#define DebugEx(CLASSNAME)    Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Debug,   __FILENAME__, __LINE__, CLASSNAME, __FUNCTION__)
#define InfoEx(CLASSNAME)     Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Info,     __FILENAME__, __LINE__, CLASSNAME, __FUNCTION__)
#define WarningEx(CLASSNAME)  Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Warning, __FILENAME__, __LINE__, CLASSNAME, __FUNCTION__)
#define ErrorEx(CLASSNAME)    Loggers::Stream(CurrentLogger::i(), AbstractLogger::ErrorClass::Error,   __FILENAME__, __LINE__, CLASSNAME, __FUNCTION__)

/**
 * @brief Current logger singleton.
 */
class CurrentLogger
{
public:

    /**
     * @brief Method for getting current global logger.
     * @return Pointer to current logger.
     */
    static LoggerPtr i();

    /**
     * @brief Method for setting current logger.
     * @param logger Current logger.
     */
    static void setCurrentLogger(LoggerPtr logger);

private:
    /**
     * @brief Hidden constructor.
     */
    CurrentLogger();

    static LoggerPtr m_logger;
};

