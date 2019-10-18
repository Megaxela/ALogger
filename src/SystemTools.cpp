#include "SystemTools.h"
#include <sstream>

#ifdef OS_LINUX
    #include <errno.h>
    #include <cstring>
    #include <sstream>
    #include <sys/stat.h>
    #include <fstream>
#endif
#ifdef OS_WINDOWS
    #include <windows.h>
#endif

uint64_t SystemTools::getLastErrorCode()
{
#ifdef OS_LINUX
    return errno;
#endif

#ifdef OS_WINDOWS
    return GetLastError();
#endif
}

std::string SystemTools::getLastErrorString()
{
#ifdef OS_LINUX
    return std::string(strerror(getLastErrorCode()));
#endif
#ifdef OS_WINDOWS
    auto error = getLastErrorCode();

    if (error)
    {
        LPVOID lpMsgBuf;
        DWORD bufLen = FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                (DWORD) error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0,
                nullptr
        );

        if (bufLen)
        {
            auto lpMsgStr = (LPCSTR)lpMsgBuf;
            std::string result(lpMsgStr, lpMsgStr+bufLen);

            LocalFree(lpMsgBuf);

            return result;
        }
    }
    return std::string();
#endif
}

std::string SystemTools::getFileContent(const std::string &path)
{
    std::stringstream result;

    std::ifstream fileStream;

    fileStream.exceptions(std::ifstream::badbit);

    // Opening file
    fileStream.open(path, std::ios::binary);

    if (!fileStream.is_open())
    {
        throw std::ifstream::failure("Can't open file \"" + path + "\".");
    }

    // Reading file content
    result << fileStream.rdbuf();

    // Closing file
    fileStream.close();

    return result.str();
}

#ifdef OS_WINDOWS
    #define PATH_SEPARATOR ('\\')
#endif
#ifdef OS_LINUX
    #define PATH_SEPARATOR ('/')
#endif

std::string SystemTools::Path::join(const std::string &l, const std::string &r)
{
    if (l[l.length() - 1] != PATH_SEPARATOR)
    {
        return (l + PATH_SEPARATOR + r);
    }
    else
    {
        return (l + r);
    }
}

bool SystemTools::Path::fileExists(const std::string &p)
{
#ifdef OS_LINUX
    struct stat buffer{};
    return (stat(p.c_str(), &buffer) == 0);
#endif

#ifdef OS_WINDOWS
    auto err = GetFileAttributes(p.c_str());
    auto lastErr = GetLastError();

    return !(INVALID_FILE_ATTRIBUTES == err && (lastErr == ERROR_FILE_NOT_FOUND || lastErr == ERROR_PATH_NOT_FOUND));
#endif
}

std::string SystemTools::Path::getDirectory(const std::string &filename)
{
    size_t found = filename.find_last_of("/\\");
    return filename.substr(0, found);
}

std::string SystemTools::Path::getFilename(const std::string &filename)
{
    size_t found = filename.find_last_of("/\\");
    return filename.substr(found + 1);
}

std::ifstream::pos_type SystemTools::Path::getFileSize(const std::string& path)
{
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}
