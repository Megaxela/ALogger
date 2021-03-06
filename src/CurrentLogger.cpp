#include "CurrentLogger.hpp"
LoggerPtr CurrentLogger::m_logger;

LoggerPtr CurrentLogger::i()
{
    if (m_logger == nullptr)
    {
        throw std::runtime_error("There is no active logger");
    }

    return m_logger;
}

CurrentLogger::CurrentLogger()
{

}

void CurrentLogger::setCurrentLogger(LoggerPtr logger)
{
    m_logger = std::move(logger);
}
