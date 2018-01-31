#include <cstring>
#include <sstream>
#include <SystemTools.h>
#include <cstdint>
#include <algorithm>
#include <map>
#include "Loggers/AbstractLogger.hpp"

AbstractLogger::AbstractLogger() :
    m_logsListeners(),
    m_maxLogFileSizeBytes(static_cast<uint64_t>(2 * 1024 * 1024)),
    m_formatString("%{DATETIME} %{FILENAME}:%{LINE} [%{THREAD}][%{CONTEXT}] %{ERROR_CLASS}: %{MESSAGE}"),
    m_formatCache(),
    m_fileLogPath("logs"),
    m_sourceFilenameTruncationEnabled(false),
    m_minTerminalOutputErrorClass(ErrorClass::Info),
    m_minFileOutputErrorClass(ErrorClass::Info),
    m_ss()
{
    cacheFormat();
}

void AbstractLogger::setMaximumLogFile(uint64_t bytes)
{
    m_maxLogFileSizeBytes = bytes;
}

uint64_t AbstractLogger::maximumLogFile() const
{
    return m_maxLogFileSizeBytes;
}

std::string AbstractLogger::classPlusFunction(std::string classname, const char *function)
{
    if (!classname.empty())
    {
        classname.append("::");
    }

    classname.append(function);

    return classname;
}

void AbstractLogger::log(AbstractLogger::ErrorClass errorClass,
                 const char *filename,
                 int line,
                 std::thread::id thread,
                 std::string classname,
                 const char *function,
                 std::string message)
{
    if (errorClass == ErrorClass::None)
    {
        return;
    }

    Message messageObject;

    // Getting current time
    messageObject.timePoint = std::chrono::system_clock::now();
    messageObject.errorClass = errorClass;
    messageObject.message = std::move(message);
    messageObject.thread = thread;
    messageObject.context = std::move(classPlusFunction(std::move(classname), function));
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

    if (!m_logsListeners.empty())
    {
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
    }

    onNewMessage(messageObject);
}

std::string AbstractLogger::messageToString(const AbstractLogger::Message& message)
{
    m_ss.seekp(std::ios::beg);

    for (auto&& cache : m_formatCache)
    {
        switch (cache.type)
        {
        case FormatCache::Type::DateTime:
        {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                message.timePoint.time_since_epoch()
            );

            auto fractional_seconds = ms.count() % 1000;

            auto time = std::chrono::system_clock::to_time_t(message.timePoint);
            std::tm* now = std::localtime(&time);

            m_ss << (now->tm_year + 1900) << '-';

            m_ss.width(2);
            m_ss.fill('0');
            m_ss << (now->tm_mon + 1)     << '-';

            m_ss.width(2);
            m_ss.fill('0');
            m_ss <<  now->tm_mday         << ' ';

            m_ss.width(2);
            m_ss.fill('0');
            m_ss <<  now->tm_hour         << ':';

            m_ss.width(2);
            m_ss.fill('0');
            m_ss <<  now->tm_min          << ':';

            m_ss.width(2);
            m_ss.fill('0');
            m_ss <<  now->tm_sec          << ',';

            m_ss.width(3);
            m_ss <<  fractional_seconds;
            break;
        }
        case FormatCache::Type::FileName:
            m_ss << message.filename;
            break;
        case FormatCache::Type::Line:
            m_ss << message.line;
            break;
        case FormatCache::Type::Thread:
        {
            m_ss << "0x";
            auto old = m_ss.fill();
            m_ss.fill('0');
            auto oldWidth = m_ss.width();
            m_ss.width(16);
            m_ss << std::hex << message.thread << std::dec;
            m_ss.fill(old);
            m_ss.width(oldWidth);
            break;
        }
        case FormatCache::Type::Context:
            m_ss << message.context;
            break;
        case FormatCache::Type::ErrorClass:
        {
            switch (message.errorClass)
            {
            case ErrorClass::Unknown:
                m_ss << "Unknown";
                break;
            case ErrorClass::Debug:
                m_ss << "Debug";
                break;
            case ErrorClass::Info:
                m_ss << "Info";
                break;
            case ErrorClass::Warning:
                m_ss << "Warning";
                break;
            case ErrorClass::Error:
                m_ss << "Error";
                break;
            case ErrorClass::None:
                log(
                    ErrorClass::Error,
                    __FILENAME__,
                    __LINE__,
                    std::this_thread::get_id(),
                    std::move(SystemTools::getTypeName(*this)),
                    __FUNCTION__,
                    "Message with error class 'None' detected. You shall not push 'None' messages."
                );
                return std::string();
            }
            break;
        }
        case FormatCache::Type::Message:
            m_ss << message.message;
            break;
        case FormatCache::Type::String:
            m_ss << cache.value;
            break;
        }
    }

    return m_ss.str();
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

    cacheFormat();
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

void AbstractLogger::cacheFormat()
{

    const std::map<std::string_view, FormatCache::Type> variables = {
        {"%{DATETIME}",    FormatCache::Type::DateTime  },
        {"%{FILENAME}",    FormatCache::Type::FileName  },
        {"%{LINE}",        FormatCache::Type::Line      },
        {"%{CONTEXT}",     FormatCache::Type::Context   },
        {"%{ERROR_CLASS}", FormatCache::Type::ErrorClass},
        {"%{MESSAGE}",     FormatCache::Type::Message   },
        {"%{THREAD}",      FormatCache::Type::Thread    },
    };

    m_formatCache.clear();

    std::string::size_type index = 0;
    std::string::size_type endIndex = 0;
    std::string::size_type previousString = 0;
    auto mapIterator = variables.end();

    while (index < m_formatString.size())
    {
        // Searching for `%{` symbol
        index = m_formatString.find("%{", index);

        // If was not found, string is finished.
        if (index == std::string::npos)
        {
            m_formatCache.emplace_back(
                FormatCache::Type::String,
                std::string_view(m_formatString.c_str() + previousString, m_formatString.size() - previousString)
            );

            break;
        }

        // Searching for close symbol
        endIndex = m_formatString.find('}', index);

        // Searching for founded expected variable
        std::string_view subString(m_formatString.c_str() + index, (endIndex + 1) - index);

        mapIterator = variables.find(subString);

        // If it's not variable
        if (mapIterator == variables.end())
        {
            index += 1;
            continue;
        }

        // If it's variable, pushing string before,
        // if it's not empty. Pushing variable
        if (previousString < index)
        {
            m_formatCache.emplace_back(
                    FormatCache::Type::String,
                    std::string_view(m_formatString.c_str() + previousString, index - previousString)
            );
        }

        // Pushing variable and moving iterators
        m_formatCache.emplace_back(
                mapIterator->second
        );

        previousString = endIndex + 1; // size of '}' = 1
        index = endIndex + 1;
    }
}
