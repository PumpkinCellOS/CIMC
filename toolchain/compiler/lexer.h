#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "cpp_compiler.h"

namespace compiler
{

struct CodePos
{
    std::string filename;
    int line;
    int column;
};

template<class T>
class LexerBase
{
public:
    virtual bool from_stream(convert::InputFile& input, const compiler::Options& options) = 0;
    virtual void display() const {}

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

}
