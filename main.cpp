#include <iostream>
#include <Loggers/BasicLogger.hpp>
#include <thread>
#include <CurrentLogger.hpp>

class Example
{
public:
    Example()
    {
        Error() << "Class example";
    }
};

int main()
{
    CurrentLogger::setCurrentLogger(std::make_shared<Logger::BasicLogger>());

    InfoF() << "Hello world";

    Example();

    return 0;
}