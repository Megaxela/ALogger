#include <cstring>
#include <sstream>
#include <SystemTools.h>
#include <cstdint>
#include <algorithm>
#include "Loggers/AbstractLogger.hpp"

AbstractLogger::AbstractLogger() :
    m_logsListeners(),
    m_maxLogFileSizeBytes(static_cast<uint64_t>(2 * 1024 * 1024)),
    m_formatString("%{DATETIME} %{FILENAME}:%{LINE} [%{CONTEXT}] %{ERROR_CLASS}: %{MESSAGE}"),
    m_fileLogPath("logs"),
    m_sourceFilenameTruncationEnabled(false),
    m_minTerminalOutputErrorClass(ErrorClass::Info),
    m_minFileOutputErrorClass(ErrorClass::Info)
{

}

void AbstractLogger::setMaximumLogFile(uint64_t bytes)
{
    m_maxLogFileSizeBytes = bytes;
}

uint64_t AbstractLogger::maximumLogFile() const
{
    return m_maxLogFileSizeBytes;
}

std::string AbstractLogger::classPlusFunction(const std::string& classname, const char *function)
{
    auto copy = classname;

    if (!copy.empty())
    {
        copy += "::";
    }

    copy += function;

    return copy;
}

void AbstractLogger::log(AbstractLogger::ErrorClass errorClass,
                 const char *filename,
                 int line,
                 const std::string& classname,
                 const char *function,
                 const std::string& message)
{
    if (errorClass == ErrorClass::None)
    {
        return;
    }

    Message messageObject;

    // Getting current time
    messageObject.timePoint = std::chrono::system_clock::now();
    messageObject.errorClass = errorClass;
    messageObject.message = message;
    messageObject.context = classPlusFunction(classname, function);
    messageObject.line = line;

    if (m_sourceFilenameTruncationEnabled)
    {
        messageObject.filename = (strrchr(filename, '/') ? strrchr(filename, '/') + 1 : filename);
    }
    else
    {
        messageObject.filename = filename;
    }

    // Removing data listeners if they have reference counter value 1
    m_logsListeners.erase(
            std::remove_if(
                    m_logsListeners.begin(),
                    m_logsListeners.end(),
                    [](Logger::LogsListenerPtr ptr)
                    {
                        return ptr.use_count() <= 1;
                    }
            ),
            m_logsListeners.end()
    );

    onNewMessage(messageObject);
}

std::string AbstractLogger::messageToString(const AbstractLogger::Message& message)
{
    auto formed = m_formatString;

    std::string::size_type searchIterator = 0;

    const std::string datetimeKey = "%{DATETIME}";
    const std::string filenameKey = "%{FILENAME}";
    const std::string lineKey = "%{LINE}";
    const std::string contextKey = "%{CONTEXT}";
    const std::string classKey = "%{ERROR_CLASS}";
    const std::string messageKey = "%{MESSAGE}";

    // DATETIME
    searchIterator = formed.find(datetimeKey, searchIterator);
    if (searchIterator != std::string::npos)
    {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            message.timePoint.time_since_epoch()
        );

        auto fractional_seconds = ms.count() % 1000;

        auto time = std::chrono::system_clock::to_time_t(message.timePoint);
        std::tm* now = std::localtime(&time);

        std::stringstream ss;

        ss << (now->tm_year + 1900) << '-';

        ss.width(2);
        ss.fill('0');
        ss << (now->tm_mon + 1)     << '-';

        ss.width(2);
        ss.fill('0');
        ss <<  now->tm_mday         << ' ';

        ss.width(2);
        ss.fill('0');
        ss <<  now->tm_hour         << ':';

        ss.width(2);
        ss.fill('0');
        ss <<  now->tm_min          << ':';

        ss.width(2);
        ss.fill('0');
        ss <<  now->tm_sec          << ',';

        ss.width(3);
        ss <<  fractional_seconds;

        formed.replace(
            searchIterator,
            datetimeKey.size(),
            ss.str()
        );
    }

    // FILENAME
    searchIterator = formed.find(filenameKey, searchIterator);
    if (searchIterator != std::string::npos)
    {
        formed.replace(
            searchIterator,
            filenameKey.size(),
            message.filename
        );
    }

    // LINE
    searchIterator = formed.find(lineKey, searchIterator);
    if (searchIterator != std::string::npos)
    {
        formed.replace(
            searchIterator,
            lineKey.size(),
            std::to_string(message.line)
        );
    }

    // CONTEXT
    searchIterator = formed.find(contextKey, searchIterator);
    if (searchIterator != std::string::npos)
    {
        formed.replace(
            searchIterator,
            contextKey.size(),
            message.context
        );
    }

    // ERROR CLASS
    searchIterator = formed.find(classKey, searchIterator);
    if (searchIterator != std::string::npos)
    {
        std::string errorClass;

        switch (message.errorClass)
        {
        case ErrorClass::Unknown:
            errorClass = "Unknown";
            break;
        case ErrorClass::Debug:
            errorClass = "Debug";
            break;
        case ErrorClass::Info:
            errorClass = "Info";
            break;
        case ErrorClass::Warning:
            errorClass = "Warning";
            break;
        case ErrorClass::Error:
            errorClass = "Error";
            break;
        case ErrorClass::None:
            log(
                ErrorClass::Error,
                __FILENAME__,
                __LINE__,
                SystemTools::getTypeName(*this),
                __FUNCTION__,
                "Message with error class 'None' detected. You shall not push 'None' messages."
            );
            return std::string();
        }

        formed.replace(
            searchIterator,
            classKey.size(),
            errorClass
        );
    }

    // MESSAGE
    searchIterator = formed.find(messageKey, searchIterator);
    if (searchIterator != std::string::npos)
    {
        formed.replace(
            searchIterator,
            messageKey.size(),
            message.message
        );
    }

    return formed;
}

void AbstractLogger::setFilenameTruncationEnabled(bool truncate)
{
    m_sourceFilenameTruncationEnabled = truncate;
}

bool AbstractLogger::filenameTruncationEnabled() const
{
    return m_sourceFilenameTruncationEnabled;
}

void AbstractLogger::setMinimumTerminalOutputErrorClass(AbstractLogger::ErrorClass errorClass)
{
    m_minTerminalOutputErrorClass = errorClass;
}

AbstractLogger::ErrorClass AbstractLogger::minimumTerminalOutputErrorClass() const
{
    return m_minTerminalOutputErrorClass;
}

void AbstractLogger::setMinimumFileOutputErrorClass(AbstractLogger::ErrorClass errorClass)
{
    m_minFileOutputErrorClass = errorClass;
}

AbstractLogger::ErrorClass AbstractLogger::minimumFileOutputErrorClass() const
{
    return m_minFileOutputErrorClass;
}

void AbstractLogger::setLogPath(std::string path)
{
    m_fileLogPath = std::move(path);
}

std::string AbstractLogger::logPath() const
{
    return m_fileLogPath;
}

void AbstractLogger::setFormat(std::string format)
{
    m_formatString = std::move(format);
}

std::string AbstractLogger::format() const
{
    return m_formatString;
}

std::string AbstractLogger::getLogPath() const
{
    // Opening current file
    std::string filename = "log.txt";

    if (m_maxLogFileSizeBytes == 0)
    {
        return filename;
    }

    std::string path = SystemTools::Path::join(m_fileLogPath, filename);

    auto filesize = static_cast<uint64_t>(SystemTools::Path::getFileSize(path));

    if (filesize > m_maxLogFileSizeBytes)
    {
        uint64_t i;
        // Renaming to new one
        for (i = 0;
             i < std::numeric_limits<uint64_t>::max() &&
             SystemTools::Path::fileExists(path + "_" + std::to_string(i + 1));
             ++i)
        {}

        // Ok, we received end
        if (i == std::numeric_limits<int>::max())
        {
            // Notify that everything is fucked up, duh
            return "";
        }

        std::rename(path.c_str(), (path + "_" + std::to_string(i + 1)).c_str());
    }

    return path;
}

void AbstractLogger::addLogsListener(Logger::LogsListenerPtr listener)
{
    m_logsListeners.push_back(listener);
}

void AbstractLogger::removeLogsListener(Logger::LogsListenerPtr listener)
{
    auto finded = std::find(
            m_logsListeners.begin(),
            m_logsListeners.end(),
            listener
    );

    if (finded == m_logsListeners.end())
    {
        return;
    }

    m_logsListeners.erase(finded);
}

void AbstractLogger::waitForLogToBeWritten()
{

}
