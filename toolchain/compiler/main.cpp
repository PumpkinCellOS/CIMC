#include <iostream>

#include <config.h>
#include <util/args.h>

int main(int argc, char* argv[])
{
    std::map<std::string, util::ArgSpec> argspec = {
        {"help",    {true,  "Display help"}},
        {"version", {true,  "Display version"}},
        {"o",       {false, "Specify output file"}}
    };
    
    util::Args args = util::parse_args(argc, argv, argspec);
    
    if(args.is_error)
        return 1;
    
    std::cout << "Options:" << std::endl;
    for(auto& it: args.options)
    {
        std::cout << it.first << " = " << it.second << std::endl;
    }
    std::cout << "Positional arguments:" << std::endl;
    for(auto& arg: args.positional_arguments)
    {
        std::cout << arg << ", " << std::endl;
    }
    
    if(args.options.count("version"))
    {
        std::cout << std::endl;
        std::cout << argv[0] << " (SPP Compiler) " << SCC_VER << " for " << SCC_OS_NAME << " (" << SCC_OS_ARCH << ")" << std::endl << std::endl;
    }
    
    return 0;
}
