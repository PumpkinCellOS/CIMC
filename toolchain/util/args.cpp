#include <util/args.h>

namespace util
{

Args parse_args(int argc, char* argv[])
{
    Args args;
    std::string current_arg;
    bool double_dash = false;
    
    for(size_t carg = 1; carg < argc; carg++)
    {
        std::string arg = argv[carg];
        if(!double_dash)
        {
            if(!current_arg.empty())
            {
                args.options[current_arg] = arg;
                continue;
            }
            else if(arg[0] == '-')
            {
                if(arg == "--")
                {
                    double_dash = true;
                    continue;
                }
                current_arg = arg;
                continue;
            }
        }
        args.positional_arguments.push_back(arg);
    }
    return args;
}

}
