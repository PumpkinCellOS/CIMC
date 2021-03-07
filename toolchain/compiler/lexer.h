#pragma once

#include "cpp_compiler.h"

#include <algorithm>
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

    CodePos codepos;
    std::string value;
    Type type = Invalid;

    void display() const;
};

class LexOutput
{
public:
    bool from_stream(convert::InputFile& input, const compiler::Options& options);
    void display();

    const Token* consume_token()
    {
        return m_index < m_tokens.size() ? &m_tokens[m_index++] : nullptr;
    }

    const Token* consume_token_of_type(Token::Type type)
    {
        if(!peek()) return nullptr;
        return peek()->type == type ? consume_token() : nullptr;
    }

    const Token* consume_token_of_types(std::vector<Token::Type> types)
    {
        if(!peek()) return nullptr;
        return std::find(types.begin(), types.end(), peek()->type) == types.end() ? nullptr : consume_token();
    }

    const Token* peek() const { return m_index < m_tokens.size() ? &m_tokens[m_index] : nullptr; }

    size_t index() const { return m_index; }
    void set_index(size_t index) { m_index = index; }

    std::istream* stream = nullptr;

private:
    std::vector<Token> m_tokens;
    size_t m_index = 0;
};

} // cpp_compiler
