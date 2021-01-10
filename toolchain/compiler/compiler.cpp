#include "compiler.h"

#include "assembler.h"
#include "cpp_compiler.h"

#include <iostream>
#include <string.h>

namespace convert
{

static bool convert_from_file(std::string input, Converter::InputFormat iformat, std::string output, Converter::OutputFormat oformat, const compiler::Options& options)
{
    std::cout << "Converting files: " << input << " > " << output << std::endl;
    
    std::ifstream ifile(input);
    if(ifile.fail())
    {
        std::cout << "- Failed to open input file: " << strerror(errno) << std::endl;
        return false;
    }
    std::ofstream ofile(output);
    if(ofile.fail())
    {
        std::cout << "- Failed to open output file: " << strerror(errno) << std::endl;
        return false;
    }
    InputFile isfile = {ifile, input};
    OutputFile osfile = {ofile, output};
    
    Converter converter(isfile, osfile);
    return converter.convert(iformat, oformat, options);
}
    
bool Converter::convert(InputFormat iformat, OutputFormat oformat, const compiler::Options& options)
{
    if(iformat == InputFormat::Cpp && oformat == OutputFormat::Asm)
    {
        return cpp_compiler::compile_to_asm(m_input, m_output, options);
    }
    else if(iformat == InputFormat::Asm && oformat == OutputFormat::Obj)
    {
        return assembler::assemble_to_obj(m_input, m_output);
    }
    std::cout << "- Unrecognized format!" << std::endl;
    return false;
}

} // convert

bool compiler_make_asm_from_file(std::string input, std::string output, const compiler::Options& options)
{
    // TODO: Detect file type!
    return convert::convert_from_file(input, convert::Converter::InputFormat::Cpp, output, convert::Converter::OutputFormat::Asm, options);
}

bool compiler_make_object_from_file(std::string input, std::string output, const compiler::Options& options)
{
    // TODO: Detect file type!
    return convert::convert_from_file(input, convert::Converter::InputFormat::Asm, output, convert::Converter::OutputFormat::Obj, options);
}
