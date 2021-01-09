#pragma once

#include <map>
#include <string>
#include <vector>

namespace util
{

struct ArgSpec
{
    bool is_bool = false;
    std::string description;
};
    
struct Args
{
    bool is_error = false;
    std::map<std::string, std::string> options;
    std::vector<std::string> positional_arguments;
};

void display_arg_help(std::map<std::string, ArgSpec> arguments);
Args parse_args(int argc, char* argv[], std::map<std::string, ArgSpec> arguments);

}
