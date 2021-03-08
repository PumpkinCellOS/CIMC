#include "lexer.h"

#include <iostream>

namespace cpp_compiler
{

namespace parse_helpers
{

bool is_simple_type(std::string word)
{
    return word == "int"
        || word == "char"
        || word == "short"
        || word == "long"
        || word == "void"
        || word == "unsigned"
        || word == "signed";
}

bool is_keyword(std::string word)
{
    return word == "struct"
        || word == "const"
        || word == "asm"
        || word == "if"
        || word == "else"
        || word == "switch"
        || word == "case"
        || word == "default"
        || word == "do"
        || word == "while"
        || word == "for"
        || word == "break"
        || word == "continue"
        || word == "return";
}

Token::Type token_type_from_word(std::string word)
{
    if(is_simple_type(word))
        return Token::TypeName;
    else if(is_keyword(word))
        return Token::Keyword;
    return Token::Name;
}

bool consume_c_comment(std::istream& stream)
{
    stream >> std::ws;
    if(stream.peek() != '/')
        return false;
    stream.ignore(1);
    char ch = stream.peek();
    switch(ch)
    {
        case '/':
            stream.ignore(STREAM_MAX, '\n');
            break;
        case '*':
            while(true)
            {
                stream.ignore(1);
                if(!stream.ignore(STREAM_MAX, '*'))
                {
                    LEX_ERROR(stream, "comment not closed");
                }
                if(stream.peek() == '/')
                {
                    stream.ignore(1);
                    break;
                }
            }
            break;
        default:
            stream.seekg(-1, std::ios::cur);
            return false;
    }
    stream >> std::ws;
    return true;
}

bool consume_word(std::istream& stream, std::string& value)
{
    std::string word;
    while(true)
    {
        int ch = stream.peek();
        if(isalnum(ch) || ch == '_')
        {
            stream.get();
        }
        else
        {
            value = word;
            return true;
        }
        word += ch;
    }
}

bool is_operator(char ch)
{
    switch(ch)
    {
    case '+':
    case '-':
    case '*':
    case '|':
    case '&':
    case '^':
    case '~':
    case '!':
    case '=':
    case ':':
    case '?':
    case '<':
    case '>':
    case '.':
    case '/':
    case '%':
        return true;
    default:
        return false;
    }
}

bool consume_operator(std::istream& stream, std::string& value)
{
    stream >> std::ws;
    std::string word;
    while(true)
    {
        int ch = stream.peek();
        if(is_operator(ch))
        {
            stream.get();
        }
        else
        {
            value = word;
            stream >> std::ws;
            return true;
        }
        word += ch;
    }
}

bool consume_number(std::istream& stream, std::string& value)
{
    std::string word;
    size_t c = 0;
    while(true)
    {
        int ch = stream.peek();
        if(isdigit(ch) || (isalnum(ch) && c > 0))
        {
            stream.get();
            c++;
        }
        else
        {
            value = word;
            return true;
        }
        word += ch;
    }
}

bool consume_string(std::istream& stream, std::string& value)
{
    if(stream.peek() != '"')
    {
        LEX_ERROR(stream, "invalid string");
    }

    stream.ignore(1);
    std::string word;
    while(true)
    {
        int ch = stream.peek();
        if(ch == '"')
        {
            stream.ignore(1);
            value = word;
            return true;
        }
        else
        {
            bool escape = false;
            if(ch == '\\')
            {
                escape = true;
                stream.get();
            }

            if(escape)
            {
                ch = stream.peek();

                // TODO: un-escape character!
                switch(ch)
                {
                    case 'n': ch = '\n'; break;
                    case 'r': ch = '\r'; break;
                    case 't': ch = '\t'; break;
                    case '\\': ch = '\\'; break;
                    case '\n': ch = '\n'; break;
                    case '"': ch = '"'; break;
                    default:
                        LEX_ERROR(stream, "invalid escape character");
                }
                escape = false;
            }

            if(stream.get() == -1)
            {
                LEX_ERROR(stream, "not-closed string");
            }
        }
        word += ch;
    }
}

} // parse_helpers

bool LexOutput::from_stream(convert::InputFile& input, const compiler::Options& options)
{
    stream = &input.stream;

    while(!input.stream.eof())
    {
        // TODO: Support line numbers and columns!d
        Token token;
        token.codepos = {input.file_name, 0, 0};
        int ch = input.stream.peek();

        // word
        if(ch == '{') // left curly
        {
            token.type = Token::LeftCurly;
            token.value = "{";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(ch == '}') // right curly
        {
            token.type = Token::RightCurly;
            token.value = "}";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(ch == '(') // left bracket
        {
            token.type = Token::LeftBracket;
            token.value = "(";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(ch == ')') // right bracket
        {
            token.type = Token::RightBracket;
            token.value = ")";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(ch == '[') // left square br.
        {
            token.type = Token::LeftSquareBracket;
            token.value = "[";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(ch == ']') // right square br.
        {
            token.type = Token::RightSquareBracket;
            token.value = "]";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(ch == '"') // string
        {
            if(!parse_helpers::consume_string(input.stream, token.value))
            {
                return false;
            }
            token.type = Token::String;
            m_tokens.push_back(token);
            input.stream >> std::ws;
        }
        else if(ch == ',') // comma
        {
            token.type = Token::Comma;
            token.value = ",";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(ch == ';') // semicolon
        {
            token.type = Token::Semicolon;
            token.value = ";";
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
        }
        else if(parse_helpers::is_operator(ch)) // operator or comment
        {
            if(!parse_helpers::consume_c_comment(input.stream))
            {
                token.type = Token::Operator;
                if(!parse_helpers::consume_operator(input.stream, token.value))
                {
                    LEX_ERROR(input.stream, "invalid operator");
                }
                input.stream >> std::ws;
                m_tokens.push_back(token);
            }
        }
        else if(isdigit(ch))
        {
            if(!parse_helpers::consume_number(input.stream, token.value))
            {
                return false;
            }

            token.type = Token::Number;
            input.stream >> std::ws;
            m_tokens.push_back(token);
        }
        else if(isalnum(ch) || ch == '_')
        {
            if(!parse_helpers::consume_word(input.stream, token.value))
            {
                return false;
            }

            token.type = parse_helpers::token_type_from_word(token.value);
            input.stream >> std::ws;
            m_tokens.push_back(token);
        }
        else
        {
            LEX_ERROR(input.stream, "invalid token");
        }
    }

    return true;
}

std::string token_type_to_color(Token::Type ttype)
{
    switch(ttype)
    {
    case Token::Comma:
    case Token::Operator:
        return "\e[92m";
    case Token::LeftBracket:
    case Token::RightBracket:
        return "\e[36m";
    case Token::Semicolon:
    case Token::LeftCurly:
    case Token::RightCurly:
        return "\e[31m";
    case Token::TypeName:
    case Token::Keyword:
        return "\e[94m";
    case Token::Name:
        return "\e[90m";
    case Token::Number:
        return "\e[95m";
    case Token::String:
        return "\e[33m";
    default:
        return "\e[91m";
    }
}

std::string token_type_to_name(Token::Type ttype)
{
    switch(ttype)
    {
    case Token::Comma: return "Comma";
    case Token::Operator: return "Operator";
    case Token::LeftBracket: return "LeftBracket";
    case Token::RightBracket: return "RightBracket";
    case Token::Semicolon: return "Semicolon";
    case Token::LeftCurly: return "LeftCurly";
    case Token::RightCurly: return "RightCurly";
    case Token::LeftSquareBracket: return "LeftSquareBracket";
    case Token::RightSquareBracket: return "RightSquareBracket";
    case Token::Keyword: return "Keyword";
    case Token::TypeName: return "TypeName";
    case Token::Name: return "Name";
    case Token::Number: return "Number";
    case Token::String: return "String";
    default: return "<invalid>";
    }
}

void Token::display() const
{
    std::cout << "Token " << codepos.filename << ":" << codepos.line << ":" << codepos.column
        << " type " << token_type_to_name(type) << " ['" << token_type_to_color(type) << value << "\e[0m']" << std::endl;
}

void LexOutput::display()
{
    for(auto& token: m_tokens)
    {
        token.display();
    }
}

}
