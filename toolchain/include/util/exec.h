#pragma once

#include <string>
#include <vector>

namespace util
{

bool spawn_process_and_wait(std::string name, std::vector<std::string> args);
    
}
