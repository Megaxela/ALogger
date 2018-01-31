#pragma once


#include <ostream>
#include "Loggers/AbstractLogger.hpp"

namespace Loggers
{
    class StreamBuffer : public std::streambuf
    {
    public:
        StreamBuffer(const StreamBuffer&) = delete;
        StreamBuffer& operator=(const StreamBuffer&) = delete;

        /**
         * @brief Constructor.
         */
        StreamBuffer();

        /**
         * @brief Destructor.
         */
        ~StreamBuffer() override = default;

        /**
         * @brief Class for pushing next message setup.
         * @param logger Logger object.
         * @param errorClass Next message error class.
         * @param filename Next message filename.
         * @param line Next message line.
         * @param thread Thread id.
         * @param classname Next message classname.
         * @param function Next message funciton.
         */
        void newMessage(LoggerPtr logger,
                        AbstractLogger::ErrorClass errorClass,
                        const char* filename,
                        int line,
                        std::thread::id thread,
                        std::string classname,
                        const char* function);

        /**
         * @brief Method for posting message to binded logger.
         */
        void postMessage();

    protected:
        int overflow(int __c) override;

    private:
        std::string m_ss;

        LoggerPtr m_logger;
        AbstractLogger::ErrorClass m_errorClass;
        const char* m_filename;
        int m_line;
        std::thread::id m_thread;
        std::string m_classname;
        const char* m_function;
    };

    /**
     * @brief Logger stream.
     */
    class Stream : public std::ostream
    {
    public:
        /**
         * @brief Constructor.
         * @param logger Pointer to logger implementation.
         * @param errorClass Error class.
         * @param filename Filename.
         * @param line Line number.
         * @param classname Class name.
         * @param function Function name.
         */
        Stream(LoggerPtr logger,
               AbstractLogger::ErrorClass errorClass,
               const char* filename,
               int line,
               std::thread::id thread,
               std::string classname,
               const char* function);

        /**
         * @brief Destructor.
         */
        ~Stream() override;

    };
}

