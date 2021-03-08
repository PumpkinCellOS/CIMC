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

namespace parse_helpers
{

bool consume_string(std::istream& stream, std::string& value);
bool consume_number(std::istream& stream, std::string& value);
bool consume_operator(std::istream& stream, std::string& value);
bool consume_word(std::istream& stream, std::string& value);
bool consume_c_comment(std::istream& stream);

}

template<class T>
class LexerBase
{
public:
    virtual bool from_stream(convert::InputFile& input, const compiler::Options& options) = 0;
    virtual void display() {}

    const T* consume_token()
    {
        return m_index < m_tokens.size() ? &m_tokens[m_index++] : nullptr;
    }

    const T* consume_token_of_type(typename T::Type type)
    {
        if(!peek()) return nullptr;
        return peek()->type == type ? consume_token() : nullptr;
    }

    const T* consume_token_of_types(std::vector<typename T::Type> types)
    {
        if(!peek()) return nullptr;
        return std::find(types.begin(), types.end(), peek()->type) == types.end() ? nullptr : consume_token();
    }

    const T* peek() const { return m_index < m_tokens.size() ? &m_tokens[m_index] : nullptr; }

    size_t index() const { return m_index; }
    void set_index(size_t index) { m_index = index; }

    std::istream* stream = nullptr;

protected:
    std::vector<T> m_tokens;
    size_t m_index = 0;
};

// TODO: Rename to CppLexer
// TODO: Rename Token to CppToken
class LexOutput : public LexerBase<Token>
{
public:
    virtual bool from_stream(convert::InputFile& input, const compiler::Options& options);
    virtual void display();
};

} // cpp_compiler
