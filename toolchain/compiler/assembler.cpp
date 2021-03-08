#include "assembler.h"

#include <iostream>

namespace assembler
{

bool assemble_to_obj(convert::InputFile& input, convert::OutputFile& output, const compiler::Options& options)
{
    std::cout << "assemble_to_obj" << std::endl;
    Lexer lexer;
    return lexer.from_stream(input, options);
    // TODO: Parser
}

bool Lexer::from_stream(convert::InputFile& input, const compiler::Options& options)
{
    std::cout << "TODO" << std::endl;
    return true;
}

}
