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

}

std::string Token::type_to_string()
{
    switch(type)
    {
        case Dot: return "Dot";
        case At: return "At";
        case String: return "String";
        case DecNumber: return "DecNumber";
        case HexNumber: return "HexNumber";
        case Name: return "Name";
        case Assignment: return "Assignment";
        case BraceOpen: return "BraceOpen";
        case BraceClose: return "BraceClose";
        case Add: return "Add";
        case Subtract: return "Subtract";
        case Invalid: return "Invalid";
        default: return "???";
    }
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
    while(!input.stream.eof())
    {
        // TODO: Support columns!
        Token token;
        token.codepos = {input.file_name, current_line, 0};
        int ch = input.stream.peek();

        switch(ch)
        {
        case '\n':
            current_line++;
            break;
        case '.':
            token.type = Token::Dot;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
            break;
        case '@':
            token.type = Token::At;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
            break;
        case '[':
            token.type = Token::BraceOpen;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
            break;
        case ']':
            token.type = Token::BraceClose;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
            break;
        case '+':
            token.type = Token::Add;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
            break;
        case '-':
            token.type = Token::Subtract;
            m_tokens.push_back(token);
            input.stream.ignore(1);
            input.stream >> std::ws;
            break;
        default:
            LEX_ERROR(input.stream, "invalid token");
        }

        // TODO:
        /*
            String, // "xxx"
            DecNumber, // 0123
            HexNumber, // 0x1AB
            Name, // abcd
            Assignment, // :=
            Invalid
        */
    }
    return true;
}

}
