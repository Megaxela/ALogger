//
// Created by megaxela on 11/8/17.
//

#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "AbstractLogger.hpp"

namespace Loggers
{
    /**
     * @brief Async logger instance.
     */
    class AsyncLogger : public AbstractLogger
    {
    public:
        /**
         * @brief Constructor.
         */
        AsyncLogger();

        /**
         * @brief Virtual destructor.
         */
        ~AsyncLogger() override;

        /**
         * @brief Method for waiting writing thread
         * to finish.
         */
        void waitForLogToBeWritten() override;

    protected:
        void onNewMessage(const Message& message) override;

    private:

        void mainThread();

        bool m_working;
        std::thread m_mainThread;

        std::queue<Message> m_messages;
        std::mutex m_messagesMutex;

        std::condition_variable m_cond;
        std::condition_variable m_clearVariable;
    };
}

