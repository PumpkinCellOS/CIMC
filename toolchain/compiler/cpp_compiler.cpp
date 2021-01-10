#include "cpp_compiler.h"

#include "lexer.h"

#include <iostream>
#include <limits>
#include <vector>

namespace cpp_compiler
{

bool compile_to_asm(convert::InputFile& input, convert::OutputFile& output, const compiler::Options& options)
{
    std::cout << "compile_to_asm" << std::endl;
    LexOutput lex_output;
    if(!lex_output.from_stream(input, options))
    {
        return false;
    }
    lex_output.display();

    return true;
}

}
