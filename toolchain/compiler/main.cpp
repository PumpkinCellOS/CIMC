#include <iostream>

#include <config.h>

#include <map>
#include <string>

std::map<std::string, std::string> parse_args(int argc, char* argv[])
{
    size_t carg = 0;
    
    // Skip program name
    carg++;
    
    std::map<std::string, std::string> argmap;
    std::string current_arg = false;
    while(carg < argc)
    {
        std::string arg = argv[carg];
        if(arg[0] == '-')
        {
            in_arg
            argmap[arg] = 
        }
    }
}

int main(int argc, char* argv[])
{
    
    return 0;
}
