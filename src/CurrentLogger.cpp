//
// Created by megaxela on 11/8/17.
//

#include "CurrentLogger.hpp"
AbstractLogger::Ptr CurrentLogger::m_logger;

AbstractLogger::Ptr CurrentLogger::i()
{
    return m_logger;
}

CurrentLogger::CurrentLogger()
{

}

void CurrentLogger::setCurrentLogger(AbstractLogger::Ptr logger)
{
    m_logger = std::move(logger);
}
