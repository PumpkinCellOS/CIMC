#pragma once

#include <map>
#include <string>
#include <vector>

namespace util
{

struct Args
{
    std::map<std::string, std::string> options;
    std::vector<std::string> positional_arguments;
};

Args parse_args(int argc, char* argv[]);

}
