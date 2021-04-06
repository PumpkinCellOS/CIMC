#pragma once

#include "../lexer.h"

namespace cpp_compiler
{

struct Token
{
    enum Type
    {
        TypeName, // int, void, char, short, long, unsigned, signed
        Keyword, // struct, const, asm, if, else, switch, case, default, do, while, for, break, continue, return
        Name, // other words
        LeftCurly, // {
        RightCurly, // }
        LeftBracket, // (
        RightBracket, // )
        Comma,
        Operator,
        Number,
        Semicolon,
        LeftSquareBracket, // [
        RightSquareBracket, // ]
        String,
        Invalid
    };

    compiler::CodePos codepos;
    std::string value;
    Type type = Invalid;

    void display() const;
};

namespace parse_helpers
{

bool consume_string(std::istream& stream, std::string& value);
bool consume_number(std::istream& stream, std::string& value);
bool consume_operator(std::istream& stream, std::string& value);
bool consume_word(std::istream& stream, std::string& value);
bool consume_c_comment(std::istream& stream);

}

// TODO: Rename to CppLexer
// TODO: Rename Token to CppToken
class LexOutput : public compiler::LexerBase<Token>
{
public:
    virtual bool from_stream(convert::InputFile& input, const compiler::Options& options);
    virtual void display();
};

} // cpp_compiler
