#pragma once

#include <fstream>
#include <string>

bool compiler_make_asm_from_file(std::string input, std::string output);
bool compiler_make_object_from_file(std::string input, std::string output);

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
