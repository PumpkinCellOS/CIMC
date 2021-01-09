#include <iostream>

#include <util/args.h>

int main(int argc, char* argv[])
{
    util::Args args = util::parse_args(argc, argv);
    if(args.is_error)
    {
        // TODO: display help!
        return 1;
    }
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
    // ...
    return 0;
}
