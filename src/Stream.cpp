#include "Stream.hpp"

Loggers::StreamBuffer::StreamBuffer() :
    m_ss(),
    m_logger(nullptr),
    m_errorClass(AbstractLogger::ErrorClass::Unknown),
    m_filename(nullptr),
    m_line(0),
    m_classname(),
    m_function(nullptr)
{

}

void Loggers::StreamBuffer::newMessage(LoggerPtr logger,
                                      AbstractLogger::ErrorClass errorClass,
                                      const char *filename,
                                      int line,
                                      const std::string& classname,
                                      const char *function)
{
    m_logger = std::move(logger);
    m_errorClass = errorClass;
    m_filename = filename;
    m_line = line;
    m_classname = classname;
    m_function = function;
}

void Loggers::StreamBuffer::postMessage()
{
    if (!m_logger)
    {
        return;
    }

    m_logger->log(
        m_errorClass,
        m_filename,
        m_line,
        m_classname,
        m_function,
        m_ss
    );

    m_ss.clear();
}

int Loggers::StreamBuffer::overflow(int __c)
{
    m_ss += (char) __c;

    return __c;
}

static thread_local Loggers::StreamBuffer streamBuffer;

Loggers::Stream::Stream(LoggerPtr logger,
                       AbstractLogger::ErrorClass errorClass,
                       const char *filename,
                       int line,
                       const std::string& classname,
                       const char *function) :
    std::ostream(&streamBuffer)
{
    streamBuffer.newMessage(std::move(logger), errorClass, filename, line, classname, function);
}

Loggers::Stream::~Stream()
{
    streamBuffer.postMessage();
}
