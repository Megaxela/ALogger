#include "CurrentLogger.hpp"
LoggerPtr CurrentLogger::m_logger;

LoggerPtr CurrentLogger::i()
{
    if (m_logger == nullptr)
    {
        std::cerr << "There is no active logger." << std::endl;
        return nullptr;
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
