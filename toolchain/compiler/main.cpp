#include "compiler.h"

#include <config.h>
#include <util/args.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::map<std::string, util::ArgSpec> argspec = {
        // Generic
        {"-help",    {true,  "Display help and quit"}},
        {"-version", {true,  "Display version and quit"}},
        
        // Output
        {"o",       {false, "Specify output file"}},
        
        // Warning options
        {"Wall",    {false, "Enable all warnings"}},
        {"Werror",  {false, "Treat warnings as errors"}},
        
        // Dialect options
        {"ffreestanding", {false, "Do not link libc and crt0 (useful for OS kernels)"}},
        
        // Misc. options
        {"S",  {false, "Do not assemble"}}
    };
    
    util::Args args = util::parse_args(argc, argv, argspec);
    
    if(args.is_error)
        return 1;
    
    if(args.options.count("-version"))
    {
        std::cout << std::endl;
        std::cout << argv[0] << " (SPP Compiler) " << SCC_VER << " for " << SCC_OS_NAME << " (" << SCC_OS_ARCH << ")" << std::endl;
        std::cout << "Licensed under No license :)" << std::endl;
        return 0;
    }
    if(args.options.count("-help"))
    {
        util::display_arg_help(argspec);
        return 0;
    }
    
    // Input file
    std::string input;
    if(args.positional_arguments.empty())
    {
        std::cout << "- No inputs specified!" << std::endl;
        return 1;
    }
    else if(args.positional_arguments.size() > 1)
    {
        std::cout << "- More than 1 input file specified!" << std::endl;
        return 1;
    }
    else
        input = args.positional_arguments[0];
    
    // Output file
    std::string output = args.options["o"];
    if(output.empty())
        output = "a.out";
    
    // ASM output
    std::string asm_output;
    bool only_asm = args.options.count("S");
    if(only_asm)
        asm_output = output;
    
    // The function will automatically detect if it's already assembly
    // basing on file extension.
    if(!compiler_make_asm_from_file(input, asm_output))
    {
        std::cout << "- Compilation failed." << std::endl;
        return 1;
    }
    if(!only_asm)
    {
        if(!compiler_make_object_from_file(asm_output, output))
        {
            std::cout << "- Assembling failed." << std::endl;
            return 1;
        }
    }
    
    return 0;
}
