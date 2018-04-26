#include <fstream>
#include <iostream>
#include "Loggers/AsyncLogger.hpp"

Loggers::AsyncLogger::AsyncLogger() :
    m_working(true),
    m_mainThread(),
    m_messages(),
    m_messagesMutex(),
    m_cond(),
    m_clearVariable()
{
    m_mainThread = std::thread(&Loggers::AsyncLogger::mainThread, this);
}

Loggers::AsyncLogger::~AsyncLogger()
{
    // Waiting until queue will be empty.
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            std::unique_lock<std::mutex> lock(m_messagesMutex);
            if (m_messages.empty())
            {
                break;
            }
        }
    }

    m_working = false;

    m_cond.notify_one();

    if (m_mainThread.joinable())
    {
        m_mainThread.join();
    }
}

void Loggers::AsyncLogger::mainThread()
{
    while (m_working)
    {
        // Locking queue
        {
            std::unique_lock<std::mutex> lock(m_messagesMutex);

            while (m_messages.empty() && m_working)
            {
                m_cond.wait(lock);
            }
        }
        // Unlocking queue

        Message message = Message();

        // Locking queue
        {
            std::ofstream file;

            std::unique_lock<std::mutex> lock(m_messagesMutex);
            while (!m_messages.empty())
            {
                message = m_messages.front();
                m_messages.pop();

                lock.unlock();

                auto stringRepresentation = messageToString(message);

                if (message.errorClass >= minimumFileOutputErrorClass())
                {
                    if (!file.is_open())
                    {
                        file.open(getLogPath(), std::ios_base::out | std::ios_base::app);
                    }

                    if (file.is_open())
                    {
                        file << stringRepresentation << std::endl;
                    }
                }

                if (message.errorClass >= minimumTerminalOutputErrorClass())
                {
                    if (message.errorClass <= ErrorClass::Info)
                    {
                        std::cout << stringRepresentation << std::endl;
                    }
                    else
                    {
                        std::cerr << stringRepresentation << std::endl;
                    }
                }

                lock.lock();
            }

            lock.unlock();

            if (file.is_open())
            {
                file.close();
            }

            m_clearVariable.notify_all();
        }
        // Unlocking queue
    }
}

void Loggers::AsyncLogger::waitForLogToBeWritten()
{
    std::unique_lock<std::mutex> lock(m_messagesMutex);

    if (m_messages.empty())
    {
        return;
    }

    while (!m_messages.empty())
    {
        m_clearVariable.wait(lock);
    }
}

void Loggers::AsyncLogger::onNewMessage(const AbstractLogger::Message& message)
{
    {
        std::unique_lock<std::mutex> lock(m_messagesMutex);
        m_messages.push(message);
    }

    m_cond.notify_one();
}
