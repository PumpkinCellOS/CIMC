#include "Config.h"

#include <mutex>

std::mutex LogStream::m_log_mutex;

LogStream log(std::string module)
{
    return LogStream(module, std::cerr);
}
