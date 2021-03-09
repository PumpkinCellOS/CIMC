#include "cpp_compiler.h"

#include "ast.h"
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

    // Parse
    auto translation_unit = std::make_shared<AST::TranslationUnit>();
    if(!translation_unit->from_lex(lex_output))
    {
        if(translation_unit)
            translation_unit->display(0);
        return false;
    }
    translation_unit->display(0);

    return true;
}

}
