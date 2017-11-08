#include <cstring>
#include <sstream>
#include <SystemTools.h>
#include <cstdint>
#include "Loggers/AbstractLogger.hpp"

AbstractLogger::AbstractLogger() :
    m_maxLogFileSizeBytes(static_cast<uint64_t>(512 * 1024)),
    m_formatString("%{DATETIME} %{FILENAME}:%{LINE} [%{CONTEXT}] %{ERROR_CLASS}: %{MESSAGE}"),
    m_fileLogPath("logs"),
    m_sourceFilenameTruncationEnabled(false),
    m_minTerminalOutputErrorClass(ErrorClass::Info),
    m_minFileOutputErrorClass(ErrorClass::Info)
{

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

    onNewMessage(messageObject);
}

std::string AbstractLogger::messageToString(const AbstractLogger::Message& message)
{
    auto formed = m_formatString;

    size_t searchIterator;

    std::string datetimeKey = "%{DATETIME}";
    std::string filenameKey = "%{FILENAME}";
    std::string lineKey = "%{LINE}";
    std::string contextKey = "%{CONTEXT}";
    std::string classKey = "%{ERROR_CLASS}";
    std::string messageKey = "%{MESSAGE}";

    // DATETIME
    searchIterator = formed.find(datetimeKey);
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
    searchIterator = formed.find(filenameKey);
    if (searchIterator != std::string::npos)
    {
        formed.replace(
            searchIterator,
            filenameKey.size(),
            message.filename
        );
    }

    // LINE
    searchIterator = formed.find(lineKey);
    if (searchIterator != std::string::npos)
    {
        formed.replace(
            searchIterator,
            lineKey.size(),
            std::to_string(message.line)
        );
    }

    // CONTEXT
    searchIterator = formed.find(contextKey);
    if (searchIterator != std::string::npos)
    {
        formed.replace(
            searchIterator,
            contextKey.size(),
            message.context
        );
    }

    // ERROR CLASS
    searchIterator = formed.find(classKey);
    if (searchIterator != std::string::npos)
    {
        std::string errorClass;

        switch (message.errorClass)
        {
        case Unknown:
            errorClass = "Unknown";
            break;
        case Debug:
            errorClass = "Debug";
            break;
        case Info:
            errorClass = "Info";
            break;
        case Warning:
            errorClass = "Warning";
            break;
        case Error:
            errorClass = "Error";
            break;
        }

        formed.replace(
            searchIterator,
            classKey.size(),
            errorClass
        );
    }

    // MESSAGE
    searchIterator = formed.find(messageKey);
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

void AbstractLogger::setLogPath(const std::string& path)
{
    m_fileLogPath = path;
}

std::string AbstractLogger::logPath() const
{
    return m_fileLogPath;
}

void AbstractLogger::setFormat(const std::string& format)
{
    m_formatString = format;
}

std::string AbstractLogger::format() const
{
    return m_formatString;
}

std::string AbstractLogger::getLogPath() const
{
    // Opening current file
    std::string filename = "log.txt";

    std::string path = SystemTools::Path::join(m_fileLogPath, filename);

    auto filesize = SystemTools::Path::getFileSize(path);

    if (filesize > m_maxLogFileSizeBytes)
    {
        int i;
        // Renaming to new one
        for (i = 0;
             i < 1024 && SystemTools::Path::fileExists(path + "_" + std::to_string(i + 1));
             ++i)
        {}

        // Ok, we received end
        if (i == 1024)
        {
            // Notify that everything is fucked up, duh
            return "";
        }

        std::rename(path.c_str(), (path + "_" + std::to_string(i + 1)).c_str());
    }

    return path;
}

