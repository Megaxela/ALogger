//
// Created by megaxela on 1/25/18.
//

#include <cstdio>
#include <zconf.h>
#include <cerrno>
#include <cstring>
#include "IostreamsLock.hpp"
#include <string>
#include <iostream>

IostreamsLock::IostreamsLock() :
    m_streambuf(std::cout.rdbuf())
{
    std::cout.rdbuf(nullptr);
}

IostreamsLock::~IostreamsLock()
{
    std::cout.rdbuf(m_streambuf);
}
