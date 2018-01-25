#pragma once

#include <string>
#include <cxxabi.h>
#include <iostream>
#include <fstream>

/**
 * @brief Some system dependent functions.
 */
namespace SystemTools
{
    /**
     * @brief Function returns last error code.
     * Code intepreting is depending on system.
     * @return Error code.
     */
    uint64_t getLastErrorCode();

    /**
     * @brief Function returns last error string.
     */
    std::string getLastErrorString();

    /**
     * @brief Function that loads file content.
     * @param path Path to file.
     * @return File content as string.
     */
    std::string getFileContent(const std::string& path);

    template<typename T>
    std::string getTypeName(const T &t)
    {
        int status;
        char* demangled = abi::__cxa_demangle(typeid(t).name(),0,0,&status);

        std::string result;
        if (status == -1)
        {
            std::cerr << "Can't allocate memory." << std::endl;
            return result;
        }
        else if (status == -2)
        {
            std::cerr << "Wrong mangled name." << std::endl;
            return result;
        }
        else if (status == -3)
        {
            std::cerr << "Some argument is invalid." << std::endl;
            return result;
        }

        result = demangled;

        free(demangled);

        return result;
    }

    namespace Path
    {
        /**
         * @brief Метод для соединения 2 путей.
         * @param l Левая часть пути.
         * @param r Правая часть пути.
         * @return Результат конкатенирования.
         */
        std::string join(const std::string &l, const std::string &r);

        /**
         * @brief Method for taking path from full filename path.
         * @param filename Full path.
         * @return Path.
         */
        std::string getDirectory(const std::string &string);

        /**
         * @brief Method for taking filename from full filename path.
         * @param string Full path.
         * @return Filename.
         */
        std::string getFilename(const std::string &string);

        /**
         * @brief Method for checking file existence.
         * @param p Path to file.
         * @return Does file exists?
         */
        bool fileExists(const std::string &p);

        /**
         * @brief Method for getting file size in bytes.
         * @param path Path to file.
         * @return File size in bytes.
         */
        std::ifstream::pos_type getFileSize(const std::string& path);
    }

    /**
     * @brief Right shift for any PC. Because of not any PC can shift more than 31 bit.
     * @tparam T Type of shifted variable.
     * @tparam IndexType Type of step.
     * @tparam Step Maximum step. (Hardware based)
     * @param v Variable to shift.
     * @param n Value of shift.
     * @return Shifted value.
     */
    template<typename T, typename IndexType=uint8_t, IndexType Step=31 >
    T shr(T v, IndexType n)
    {
        for (IndexType i = 0; i < n; i += std::min(static_cast<IndexType>(n - i), static_cast<IndexType>(Step)))
        {
            v >>= std::min(static_cast<IndexType>(n - i), static_cast<IndexType>(Step));
        }

        return v;
    }

    /**
     * @brief Left shift for any PC. Because of not any PC can shift more than 31 bit.
     * @tparam T Type of shifted variable.
     * @tparam IndexType Type of step.
     * @tparam Step Maximum step. (Hardware based)
     * @param v Variable to shift.
     * @param n Value of shift.
     * @return Shifted value.
     */
    template<typename T, typename IndexType=uint8_t, IndexType Step=31 >
    T shl(T v, IndexType n)
    {
        for (IndexType i = 0; i < n; i += std::min(static_cast<IndexType>(n - i), static_cast<IndexType>(Step)))
        {
            v <<= std::min(static_cast<IndexType>(n - i), static_cast<IndexType>(Step));
        }

        return v;
    }
}
