#include <util/args.h>

#include <iostream>

namespace util
{

Args parse_args(int argc, char* argv[], std::map<std::string, ArgSpec> arguments)
{
    Args args;
    bool double_dash = false;
    
    for(size_t carg = 1; carg < argc; carg++)
    {
        std::string arg = argv[carg];
        if(arg[0] == '-' && !double_dash)
        {
            if(arg == "--")
            {
                double_dash = true;
                continue;
            }
            
            auto it = arguments.find(arg.substr(1));
            if(it == arguments.end())
            {
                std::cout << "Invalid option: " << arg << std::endl;
                args.is_error = true;
                return args;
            }
            
            ArgSpec argspec = it->second;
            if(!argspec.is_bool)
            {
                carg++;
                if(carg >= argc)
                {
                    std::cout << "Invalid command line at arg " << carg << std::endl;
                    args.is_error = true;
                    return args;
                }
                std::string value = argv[carg];
                args.options[arg] = value;
            }
            else
            {
                args.options[arg] = "1";
            }
        }
        else
        {
            args.positional_arguments.push_back(arg);
            double_dash = true;
        }
    }
    return args;
}

}
