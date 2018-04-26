#include <iostream>
#include "Loggers/BasicLogger.hpp"

Loggers::BasicLogger::BasicLogger() :
    m_outputFile(),
    m_fileMutex(),
    m_terminalMutex()
{

}

void Loggers::BasicLogger::onNewMessage(const AbstractLogger::Message& message)
{
    if (message.errorClass < minimumFileOutputErrorClass() &&
        message.errorClass < minimumTerminalOutputErrorClass())
    {
        return;
    }

    // Generating string
    auto msg = std::move(messageToString(message));

    // Writing to file first (minimumFileOutputErrorClass is not thread safe)
    if (message.errorClass >= minimumFileOutputErrorClass())
    {
        std::unique_lock<std::mutex> lock(m_fileMutex);
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
        std::unique_lock<std::mutex> lock(m_terminalMutex);
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
