#pragma once


#include <fstream>
#include <mutex>
#include "AbstractLogger.hpp"

namespace Logger
{
    /**
     * @brief Default basic synchronous logger.
     */
    class BasicLogger : public AbstractLogger
    {
    public:
        /**
         * @brief Constructor.
         */
        BasicLogger();

    protected:
        /**
         * @brief On new message, this logger, synchronous writes
         * to terminal and/or file.
         * @param message Message object.
         */
        void onNewMessage(const Message& message) override;

    private:
        std::ofstream m_outputFile;

        std::mutex m_fileMutex;
        std::mutex m_terminalMutex;
    };
}
