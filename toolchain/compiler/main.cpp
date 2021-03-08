#include "compiler.h"

#include <config.h>
#include <util/args.h>

#include <iostream>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    std::map<std::string, util::ArgSpec> argspec = {
        // Generic
        {"-help",    {true,  "Display help and quit"}},
        {"-version", {true,  "Display version and quit"}},

        // Input
        {"x",       {false, "Specify input language. Possible values are c,asm."}},

        // Output
        {"o",       {false, "Specify output file"}},

        // Warning options
        {"Wall",    {true, "Enable all warnings"}},
        {"Werror",  {true, "Treat warnings as errors"}},

        // Dialect options
        {"ffreestanding", {true, "Do not link libc and crt0 (useful for OS kernels)"}},

        // Misc. options
        {"S",  {true, "Only compile, do not assemble. It's ignored if input language is 'asm'"}}
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
    else
    {
        mkdir(SCC_TMP_PATH, 0700);
        asm_output = input;

        // Replace all special characters with '_' in asm_output
        for(char& ch : asm_output)
        {
            if(ch == '/' || ch == '.' || ch == '~')
                ch = '_';
        }
        asm_output = std::string(SCC_TMP_PATH) + "/" + asm_output + ".asm";
    }

    compiler::Options options;

    // Warnings
    if(args.options.count("Wall"))
        options.w_enable_all = true;
    if(args.options.count("Werror"))
        options.w_treat_as_errors = true;

    // Dialect options
    if(args.options.count("ffreestanding"))
        options.f_freestanding = true;

    // Input
    std::string language = args.options["x"];
    if(language.empty() || language == "c")
    {
        if(!compiler_make_asm_from_file(input, asm_output, options))
        {
            std::cout << "- Compilation failed." << std::endl;
            return 1;
        }

        if(!only_asm)
        {
            if(!compiler_make_object_from_file(asm_output, output, options))
            {
                std::cout << "- Assembling failed." << std::endl;
                return 1;
            }
        }
    }
    else if(language == "asm")
    {
        if(!compiler_make_object_from_file(input, output, options))
        {
            std::cout << "- Assembling failed." << std::endl;
            return 1;
        }
    }

    return 0;
}
