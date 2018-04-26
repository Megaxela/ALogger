# Simple C++ logging system
It's pure C++11 based simple logging system.

## Build
It's CMake based project, it can be used as 
subproject (`add_subdirectory`) in your CMake project.

But also you can build it and use in any build system.

Steps to build it:
1. Clone repo: `git clone https://github.com/Megaxela/ALogger`
1. Go into cloned repo: `cd ALogger`
1. Create build folder `mkdir build`
1. Go into build folder `cd build`
1. Setup project: `cmake ..`
1. Build library: `cmake --build` or `make`

## Usage example
First of all it's logging system, so you can setup it globally or use 
separately.

### Separately example


### Global example
```cpp
#include <CurrentLogger.hpp>
#include <Loggers/BasicLogger.hpp>

class ExampleClass
{
public:
    ExampleClass()
    {
        Log() << "Constructor";
    }
}

int main(int argc, char** argv)
{
    // Setting current logger
    // BasicLogger - it's default blocking logger.
    CurrentLogger::setCurrentLogger(
        std::make_shared<Loggers::BasicLogger>()
    );
    
    // You can define your own format string.
    // Default format string is
    // "%{DATETIME} %{FILENAME}:%{LINE} [%{THREAD}][%{CONTEXT}] %{ERROR_CLASS}: %{MESSAGE}"
    CurrentLogger::i()->setFormat("[%{DATETIME}] %{FILENAME}:%{LINE} [%{CONTEXT}] <%{THREAD}> {%{ERROR_CLASS}}: %{MESSAGE}");
    
    // Log has filename, where log was called, so it 
    // can be truncated (until filename), or has full
    // filename.
    CurrentLogger::i()->setFilenameTruncationEnabled(false);
    
    // You can define minimum error class to output into 
    // file or terminal.
    // Default path to file logs "./logs"
    CurrentLogger::i()->setMinimumFileOutputErrorClass(AbstractLogger::ErrorClass::Info);
    CurrentLogger::i()->setMinimumTerminalOutputErrorClass(AbstractLogger::ErrorClass::Info);
    
    // Cause `main` it's function, so you has to call global macros
    // with `F` postfix (to skip classname identification).
    LogF() << "Initilized";
    
    ExampleClass();
}
```

## LICENSE

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

Library is licensed under the [MIT License](https://opensource.org/licenses/MIT) 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.