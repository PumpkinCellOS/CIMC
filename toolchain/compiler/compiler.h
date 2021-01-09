#pragma once

#include <fstream>
#include <string>

namespace cpp_compiler
{

struct Options
{
    // Warnings
    bool w_enable_all = false;
    bool w_treat_as_errors = false;
    
    // Dialect options
    bool f_freestanding = false;
};
    
}

bool compiler_make_asm_from_file(std::string input, std::string output);
bool compiler_make_object_from_file(std::string input, std::string output, const cpp_compiler::Options& options);

namespace convert
{

struct InputFile
{
    std::istream& stream;
    std::string file_name;
};
  
struct OutputFile
{
    std::ostream& stream;
    std::string file_name;
};

class Converter
{
public:
    Converter(const InputFile& input, const OutputFile& output)
    : m_input(input), m_output(output) {}
    
    enum class InputFormat
    {
        Cpp,
        Asm
    };
    enum class OutputFormat
    {
        Asm,
        Obj
    };
    
    bool convert(InputFormat iformat, OutputFormat oformat);
    
private:
    InputFile m_input;
    OutputFile m_output;
};

} // convert
