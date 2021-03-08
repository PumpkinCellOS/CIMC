#include "assembler.h"

#include <iostream>

namespace assembler
{

bool assemble_to_obj(convert::InputFile& input, convert::OutputFile& output, const compiler::Options& options)
{
    std::cout << "assemble_to_obj" << std::endl;
    Lexer lexer;
    bool success = lexer.from_stream(input, options);
    if(success)
    {
        lexer.display();
        return true;
    }
    return false;
    // TODO: Parser
}

namespace parse_helpers
{

void consume_whitespace_except_newline(std::istream& input)
{
    int ch = input.peek();
    while(isspace(ch) && ch != '\n')
    {
        std::cout << "W'" << ch << "' ";
        input.ignore(1);
        ch = input.peek();
    }
}

bool consume_hex_number(std::istream& input, std::string& value)
{
    // TODO:
    int ch = input.peek();
    while(isdigit(ch) || (toupper(ch) >= 'A' && toupper(ch) <= 'F'))
    {
        std::cout << "H'" << (char)ch << "' ";
        value += (char)ch;
        std::cout << value << std::endl;
        input.ignore(1);
        ch = input.peek();
    }
    return true;
}

}

std::string Token::type_to_string()
{
    switch(type)
    {
        case Dot: return "Dot";
        case At: return "At";
        case Comma: return "Comma";
        case String: return "String";
        case DecNumber: return "DecNumber";
        case HexNumber: return "HexNumber";
        case Name: return "Name";
        case Assignment: return "Assignment";
        case BraceOpen: return "BraceOpen";
        case BraceClose: return "BraceClose";
        case Add: return "Add";
        case Subtract: return "Subtract";
        case NewLine: return "NewLine";
        case Colon: return "Colon";
        case Invalid: return "Invalid";
    }
    return std::to_string(type);
}

void Lexer::display()
{
    for(auto& token: m_tokens)
    {
        std::cout << "Token " << token.type_to_string() << " ['" << token.value << "'] " << std::endl;
    }
}

bool Lexer::from_stream(convert::InputFile& input, const compiler::Options& options)
{
    int current_line = 1;
    parse_helpers::consume_whitespace_except_newline(input.stream); // First indent
    while(!input.stream.eof())
    {
        // TODO: Support columns!
        // TODO: Support comments ( '; abc' )
        Token token;
        token.codepos = {input.file_name, current_line, 0};
        int ch = input.stream.peek();

        std::cout << "'" << (char)ch << "' ";

        switch(ch)
        {
        case '\n':
            token.type = Token::NewLine;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            current_line++;
            parse_helpers::consume_whitespace_except_newline(input.stream); // Indent
            break;
        case '.':
            token.type = Token::Dot;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case '@':
            token.type = Token::At;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case ',':
            token.type = Token::Comma;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case '[':
            token.type = Token::BraceOpen;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case ']':
            token.type = Token::BraceClose;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case '+':
            token.type = Token::Add;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case '-':
            token.type = Token::Subtract;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case '"':
            token.type = Token::String;
            if(!cpp_compiler::parse_helpers::consume_string(input.stream, token.value))
                return false;
            m_tokens.push_back(token);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        case ':':
            input.stream.ignore(1);
            if(input.stream.peek() == '=')
            {
                token.type = Token::Assignment;
                input.stream.ignore(1);
            }
            else
                token.type = Token::Colon;
            m_tokens.push_back(token);
            parse_helpers::consume_whitespace_except_newline(input.stream);
            break;
        default:
            {
                if(isdigit(ch))
                {
                    char data[2];
                    input.stream.read(data, 2);
                    if(data[0] == '0' && data[1] == 'x')
                    {
                        token.type = Token::HexNumber;
                        if(!parse_helpers::consume_hex_number(input.stream, token.value))
                            LEX_ERROR(input.stream, "invalid hex number");
                        std::cout << "Value(" << token.value << ")" << std::endl;
                        m_tokens.push_back(token);
                    }
                    else
                    {
                        input.stream.seekg(-2, std::ios::cur);
                        token.type = Token::DecNumber;
                        if(!cpp_compiler::parse_helpers::consume_number(input.stream, token.value))
                            LEX_ERROR(input.stream, "invalid number");
                        m_tokens.push_back(token);
                    }
                    parse_helpers::consume_whitespace_except_newline(input.stream);
                }
                else if(isalnum(ch) || ch == '_')
                {
                    token.type = Token::Name;
                    if(!cpp_compiler::parse_helpers::consume_word(input.stream, token.value))
                        LEX_ERROR(input.stream, "invalid word");
                    m_tokens.push_back(token);
                    parse_helpers::consume_whitespace_except_newline(input.stream);
                }
                else
                    LEX_ERROR(input.stream, "invalid token");
            }
            break;
        }
    }
    return true;
}

}
