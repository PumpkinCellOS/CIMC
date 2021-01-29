#pragma once

#include <atomic>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;

struct LogStream
{
public:
    LogStream(std::string module, std::ostream& stream, std::string format = "0")
    : m_stream(stream)
    {
        m_log_mutex.lock();
        std::cerr << "\e[" + format + ";1m" << module << "\e[0;" << format << "m: ";
    }

    ~LogStream() { std::cerr << std::endl; m_log_mutex.unlock(); }

    template<class T>
    LogStream& operator<<(T&& t)
    {
        m_stream << t;
        return *this;
    }

private:
    static std::mutex m_log_mutex;
    std::ostream& m_stream;
};

LogStream info(std::string module);
LogStream error(std::string module);
LogStream debug(std::string module);
LogStream trace(std::string module);
