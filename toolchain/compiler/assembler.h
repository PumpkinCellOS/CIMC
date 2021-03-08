#pragma once

#include "compiler.h"

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
        String, // "xxx"
        DecNumber, // 0123
        HexNumber, // 0x1AB
        Name, // abcd
        Assignment, // :=
        BraceOpen, // [
        BraceClose, // ]
        Add, // +
        Subtract, // -
        Invalid
    };

    cpp_compiler::CodePos codepos;
    std::string value;
    Type type = Invalid;

    void display() const;
};

class Lexer : public cpp_compiler::LexerBase<Token>
{
public:
    virtual bool from_stream(convert::InputFile& input, const compiler::Options& options);
    virtual void display() {}
};

bool assemble_to_obj(convert::InputFile& input, convert::OutputFile& output, const compiler::Options& options);

} // cpp_compiler
