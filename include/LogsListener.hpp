#pragma once

#include <memory>
#include <Loggers/AbstractLogger.hpp>

namespace Logger
{
    class LogsListener;

    using LogsListenerPtr = std::shared_ptr<LogsListener>;

    /**
     * @brief Class, that describes
     * base for logs listener.
     */
    class LogsListener
    {
    public:

        /**
         * @brief Virtual destructor.
         */
        virtual ~LogsListener() = default;

        /**
         * @brief Method for popping message from
         * listener queue.
         * @return Message.
         */
        virtual AbstractLogger::Message popMessage() = 0;

        /**
         * @brief Method for checking is there
         * any messages inside listener.
         * @return Is messages inside.
         */
        virtual bool hasMessages() const = 0;

        /**
         * @brief Method for adding new messages.
         * Logger will use this method.
         * @param m Message.
         */
        virtual void newMessage(const AbstractLogger::Message& m) = 0;
    };
}