#pragma once

#include "cpp_compiler.h"

#include <string>
#include <vector>

namespace cpp_compiler
{

struct CodePos
{
    std::string filename;
    int line;
    int column;
};

struct Token
{
    enum Type
    {
        TypeName, // int, void, char, short, long
        Keyword, // struct, const, asm, if, else, switch, case, default, do, while, for, break, continue, return
        Name, // other words
        LeftCurly,
        RightCurly,
        LeftBracket,
        RightBracket,
        Comma,
        Operator,
        Number,
        Semicolon,
        LeftSquareBracket,
        RightSquareBracket,
        String,
        Invalid
    };

    CodePos codepos;
    std::string value;
    Type type = Invalid;
};

class LexOutput
{
public:
    bool from_stream(convert::InputFile& input, const compiler::Options& options);
    void display();

private:
    std::vector<Token> m_tokens;
};

} // cpp_compiler
