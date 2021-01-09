#include "compiler.h"

#include <iostream>

bool compiler_make_asm_from_file(std::string input, std::string output)
{
    std::ifstream ifile(input);
    if(!ifile.good())
    {
        std::cout << "- Failed to open input file!" << std::endl;
        return false;
    }
    std::ofstream ofile(output);
    if(!ofile.good())
    {
        std::cout << "- Failed to open output file!" << std::endl;
        return false;
    }
    return convert::cpp_to_asm(ifile, ofile);
}

bool compiler_make_object_from_file(std::string input, std::string output)
{
    std::ifstream ifile(input);
    if(!ifile.good())
    {
        std::cout << "- Failed to open input file!" << std::endl;
        return false;
    }
    std::ofstream ofile(output);
    if(!ofile.good())
    {
        std::cout << "- Failed to open output file!" << std::endl;
        return false;
    }
    return convert::asm_to_obj(ifile, ofile);
}

namespace convert
{
    
bool cpp_to_asm(std::ifstream& input, std::ofstream& output)
{
    std::cout << "cpp_to_asm" << std::endl;
    return true;
}
bool asm_to_obj(std::ifstream& input, std::ofstream& output)
{
    std::cout << "asm_to_obj" << std::endl;
    return true;
}

}
