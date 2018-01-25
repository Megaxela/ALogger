//
// Created by megaxela on 1/25/18.
//

#pragma once


#include <streambuf>

class IostreamsLock
{
public:
    IostreamsLock();

    ~IostreamsLock();
private:
    std::streambuf* m_streambuf;
};

