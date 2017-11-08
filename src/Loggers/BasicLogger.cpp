#include <iostream>
#include "Loggers/BasicLogger.hpp"

Logger::BasicLogger::BasicLogger() :
    m_outputFile()
{

}

void Logger::BasicLogger::onNewMessage(const AbstractLogger::Message& message)
{
    // Generating string
    auto msg = messageToString(message);

    // Writing to file first (minimumFileOutputErrorClass is not thread safe)
    if (message.errorClass >= minimumFileOutputErrorClass())
    {
        std::lock_guard<std::mutex> lock(m_fileMutex);
        if (m_outputFile.is_open())
        {
            m_outputFile.close();
        }

        m_outputFile.open(getLogPath(), std::ios_base::out | std::ios_base::app);

        if (m_outputFile.is_open())
        {
            m_outputFile << msg << std::endl;
            m_outputFile.close();
        }
    }

    // Writing to terminal   (minimumTerminalOutputErrorClass is not thread safe)
    if (message.errorClass >= minimumTerminalOutputErrorClass())
    {
        std::lock_guard<std::mutex> lock(m_terminalMutex);
        if (message.errorClass <= ErrorClass::Info)
        {
            std::cout << msg << std::endl;
        }
        else
        {
            std::cerr << msg << std::endl;
        }
    }
}
