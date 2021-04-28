#pragma once

#include "../lexer.h"
#include "lexer.h"

namespace assembler
{

class Token
{
public:
    enum Type
    {
        Dot, // .
        At, // @
        Colon, // :
        Comma, // ,
        String, // "xxx"
        DecNumber, // 0123
        HexNumber, // 0x1AB
        Name, // abcd
        Assignment, // :=
        BraceOpen, // [
        BraceClose, // ]
        Add, // +
        Subtract, // -
        Hash, // #
        NewLine, // \n
        Invalid
    };

    compiler::CodePos codepos;
    std::string value;
    Type type = Invalid;

    std::string type_to_string() const;
    void display() const;
};

class Lexer : public compiler::LexerBase<Token>
{
public:
    virtual bool from_stream(convert::InputFile& input, const compiler::Options& options) override;
    virtual void display() const override;
};

bool assemble_to_obj(convert::InputFile& input, convert::OutputFile& output, const compiler::Options& options);

} // assembler
