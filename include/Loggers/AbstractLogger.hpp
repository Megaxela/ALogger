#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <cstdint>
#include <vector>
#include <sstream>
#include <thread>
#include <string_view>

#ifdef OS_LINUX
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else //OS_WINDOWS
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

class AbstractLogger;

using LoggerPtr = std::shared_ptr<AbstractLogger>;

namespace Logger
{
    class LogsListener;

    using LogsListenerPtr = std::shared_ptr<LogsListener>;
}

/**
 * @brief Class, that describes
 * abstract logger object.
 */
class AbstractLogger
{
public:

    enum class ErrorClass
    {
        Unknown    //< Service error class.
        , Debug    //< Information that is exceed in normal working process
        , Info     //< Information that has to be some point messages.
        , Warning  //< Information that something was not good, but it was not critical or could be fixed automatically.
        , Error    //< Information that some error was acquired and application has to be closed.
        , None     //< Messages with None error class will be ignored. This value has to be passed to 'minimumErrorClass' setters to suppress any output.
    };

    /**
     * @brief Struct, that
     * describes logger message.
     */
    struct Message
    {
        /**
         * @brief Default constructor.
         */
        Message() :
            timePoint(),
            errorClass(ErrorClass::Unknown),
            message(),
            thread(),
            filename(nullptr),
            context(),
            line(0)
        {}

        Message(const Message&) = default;
        Message& operator=(const Message&) = default;

        std::chrono::system_clock::time_point timePoint;
        ErrorClass errorClass;
        std::string message;
        std::thread::id thread;
        const char* filename;
        std::string context;
        int line;
    };

    /**
     * @brief Basic constructor.
     */
    AbstractLogger();

    /**
     * @brief Defult destructor.
     */
    virtual ~AbstractLogger() = default;

    /**
     * @brief Method for putting some information into logger.
     * @param errorClass Log error class enum value.
     * @param message Log message.
     * @param file Filename.
     * @param classname Log context. For example class and method names or function name.
     * @param line Line in source code.
     * @param thread Callee thread id.
     */
    void log(AbstractLogger::ErrorClass errorClass,
             const char *filename,
             int line,
             std::thread::id thread,
             std::string classname,
             const char *function,
             std::string message);

    /**
     * @brief Method for setting is truncation of
     * source filename enabled. If enabled log message
     * will contain only filename, without path.
     * @param truncate Is truncation enabled.
     */
    void setFilenameTruncationEnabled(bool truncate);

    /**
     * @brief Method for getting is truncation of
     * source filename enable.
     * @return Is truncation enabled.
     */
    bool filenameTruncationEnabled() const;

    /**
     * @brief Method for setting minimum
     * message output class for terminal output.
     * @param errorClass Error class enum value.
     */
    void setMinimumTerminalOutputErrorClass(ErrorClass errorClass);

    /**
     * @brief Method for getting minimum
     * message output class for terminal output.
     * @return Error class enum value.
     */
    ErrorClass minimumTerminalOutputErrorClass() const;

    /**
     * @brief Method for setting minimum
     * message output class for output to file.
     * @param errorClass Error class enum value.
     */
    void setMinimumFileOutputErrorClass(ErrorClass errorClass);

    /**
     * @brief Method for getting minimum
     * message output class for output to file.
     * @return Error class enum value.
     */
    ErrorClass minimumFileOutputErrorClass() const;

    /**
     * @brief Method for setting path to directory
     * where all file logs will contains. File logs
     * are rolling logs. If there will no such directory
     * there will no any file logs. Default value is "logs".
     * @param path Path to directory.
     */
    void setLogPath(std::string path);

    /**
     * @brief Method for getting path to directory
     * where all file logs will contains. Default value is "logs".
     * @return Path to directory.
     */
    std::string logPath() const;

    /**
     * @brief Method for setting format string for
     * logger.
     * Possible fields:
     * %{DATETIME}    - date and time info.
     * %{FILENAME}    - source filename.
     * %{LINE}        - source file line.
     * ${THREAD}      - thread context.
     * %{CONTEXT}     - call context.
     * %{ERROR_CLASS} - error class name.
     * %{MESSAGE}     - error message
     * @param format Format string.
     */
    void setFormat(std::string format);

    /**
     * @brief Method for getting current format
     * string.
     * @return Format string.
     */
    std::string format() const;

    /**
     * @brief Method for adding log listener.
     * @param listener Listener.
     */
    void addLogsListener(Logger::LogsListenerPtr listener);

    /**
     * @brief Method for removing logs listener.
     * @param listener Listener.
     */
    void removeLogsListener(Logger::LogsListenerPtr listener);

    /**
     * @brief Method for waiting logs to be written.
     */
    virtual void waitForLogToBeWritten();

    /**
     * @brief Method for setting maximum log
     * file size in bytes. If log file
     * will be bigger than maximum value
     * log rotation will be applied. If
     * value will be 0 - there will no
     * rotation.
     * @param bytes Number of bytes.
     */
    void setMaximumLogFile(uint64_t bytes);

    /**
     * @brief Method for getting maximum log
     * file size.
     * @return Number of bytes.
     */
    uint64_t maximumLogFile() const;

protected:

    /**
     * @brief Method that's called on new log message after
     * some basic processing.
     * @param message Message object.
     */
    virtual void onNewMessage(const Message& message) = 0;

    /**
     * @brief Method for transforming message object
     * to string, depending on format string.
     * @param message Message object.
     * @return Formatted log string.
     * @todo Can be optimized to O(n).
     */
    std::string messageToString(const Message& message);

    /**
     * @brief Method for getting log file path, using
     * log file rotation.
     * @return
     */
    std::string getLogPath() const;

private:

    /**
     * @brief Method for caching
     * current format string to
     * optimize string forming.
     */
    void cacheFormat();

    struct FormatCache
    {
        /*
         * %{DATETIME}    - date and time info.
         * %{FILENAME}    - source filename.
         * %{LINE}        - source file line.
         * ${THREAD}      - thread context.
         * %{CONTEXT}     - call context.
         * %{ERROR_CLASS} - error class name.
         * %{MESSAGE}     - error message
         */
        enum class Type
        {
            DateTime,
            FileName,
            Line,
            Thread,
            Context,
            ErrorClass,
            Message,
            String
        };

        explicit FormatCache(Type type) :
            type(type),
            value()
        {}

        FormatCache(Type type, std::string_view view) :
            type(type),
            value(view)
        {}

        Type type;
        std::string_view value;
    };

    std::string classPlusFunction(std::string classname, const char* function);

    std::vector<Logger::LogsListenerPtr> m_logsListeners;

    uint64_t m_maxLogFileSizeBytes;
    std::string m_formatString;
    std::vector<FormatCache> m_formatCache;
    std::string m_fileLogPath;
    bool m_sourceFilenameTruncationEnabled;
    ErrorClass m_minTerminalOutputErrorClass;
    ErrorClass m_minFileOutputErrorClass;
    std::stringstream m_ss;
};

