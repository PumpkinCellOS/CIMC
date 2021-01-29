#include "Config.h"

#include <mutex>

std::mutex LogStream::m_log_mutex;

LogStream info(std::string module) { return LogStream(module, std::cerr, "36"); }
LogStream error(std::string module) { return LogStream(module, std::cerr, "31"); }
LogStream debug(std::string module) { return LogStream(module, std::cerr, "37"); }
LogStream trace(std::string module) { return LogStream(module, std::cerr, "90"); }
