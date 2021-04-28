#include "parser.h"

namespace assembler
{

bool ignore_at_least_one_of_type(Lexer& lexer, Token::Type type)
{
    const Token* token;
    size_t counter = 0;
    do counter++; while(token = lexer.consume_token_of_type(type));
    if(counter == 0)
    {
        return false;
    }
    return true;
}

void ignore_all_of_type(Lexer& lexer, Token::Type type)
{
    const Token* token;
    while(token = lexer.consume_token_of_type(type));
}

// operand ::= address-operand | section-operand | isr-operand | number | identifier
std::shared_ptr<Operand> parse_operand(Lexer& lexer)
{
    std::shared_ptr<Operand> operand;
    size_t pos = lexer.index();

    operand = parse_address_operand(lexer);
    if(!operand)
    {
        lexer.set_index(pos);
        operand = parse_section_operand(lexer);
        if(!operand)
        {
            lexer.set_index(pos);
            operand = parse_isr_operand(lexer);
            if(!operand)
            {
                lexer.set_index(pos);
                operand = std::make_shared<Operand>();
                auto token = lexer.consume_token_of_types({Token::DecNumber, Token::HexNumber, Token::Name, Token::String});
                if(!token)
                {
                    operand->type = Operand::Type::Invalid;
                    return operand;
                }

                switch(token->type)
                {
                case Token::DecNumber:
                    operand->type = Operand::Type::DecNumber;
                case Token::HexNumber:
                    operand->type = Operand::Type::HexNumber;
                case Token::Name:
                    operand->type = Operand::Type::Name;
                case Token::String:
                    operand->type = Operand::Type::String;
                }
                operand->value = token->value;
            }
        }
    }
    return operand;
}

// identifier ::= [ relative-address | name ]
std::shared_ptr<Identifier> parse_identifier(Lexer& lexer)
{
    std::shared_ptr<Identifier> identifier;
    size_t pos = lexer.index();
    identifier = parse_relative_address(lexer);
    if(!identifier)
    {
        lexer.set_index(pos);
        auto name_identifier = std::make_shared<NameIdentifier>();
        auto name_token = lexer.consume_token_of_type(Token::Name);
        if(!name_token)
        {
            lexer.set_index(pos);
            return nullptr;
        }

        name_identifier->name = name_token->value;
        identifier = name_identifier;
    }
    return identifier;
}

// relative-address ::= name [+-] number
std::shared_ptr<RelativeAddress> parse_relative_address(Lexer& lexer)
{
    auto name = lexer.consume_token_of_type(Token::Name);
    if(!name)
        return nullptr;

    auto sign = lexer.consume_token_of_types({Token::Add, Token::Subtract});
    if(!sign)
        return nullptr;

    // TODO: Support hex numbers
    auto offset_token = lexer.consume_token_of_type(Token::DecNumber);
    if(!offset_token)
        return nullptr;

    int offset = 0;

    try
    {
        offset = std::stoi(offset_token->value);
    }
    catch(...)
    {
        PARSE_ERROR(lexer, "invalid number in relative address");
    }

    if(sign->type == Token::Subtract)
        offset = -offset;

    std::shared_ptr<RelativeAddress> relative_address = std::make_shared<RelativeAddress>();
    relative_address->name = name->value;
    relative_address->offset = offset;
    return relative_address;
}

// address-operand ::= [ identifier ]
std::shared_ptr<AddressOperand> parse_address_operand(Lexer& lexer)
{
    auto l_brace = lexer.consume_token_of_type(Token::BraceOpen);
    if(!l_brace)
        return nullptr;

    auto identifier = parse_identifier(lexer);
    if(!identifier)
        return nullptr;

    auto r_brace = lexer.consume_token_of_type(Token::BraceClose);
    if(!r_brace)
        return nullptr;

    auto address_operand = std::make_shared<AddressOperand>();
    address_operand->identifier = identifier;
    return address_operand;
}

std::shared_ptr<ISROperand> parse_isr_operand(Lexer& lexer)
{
    auto hash = lexer.consume_token_of_type(Token::Hash);
    if(!hash)
        return nullptr;

    auto value = lexer.consume_token_of_type(Token::Name);
    if(!value)
        return nullptr;

    std::shared_ptr<ISROperand> operand = std::make_shared<ISROperand>();
    operand->value = value->value;
    return operand;
}

std::shared_ptr<SectionOperand> parse_section_operand(Lexer& lexer)
{
    auto hash = lexer.consume_token_of_type(Token::Dot);
    if(!hash)
        return nullptr;

    auto value = lexer.consume_token_of_type(Token::Name);
    if(!value)
        return nullptr;

    std::shared_ptr<SectionOperand> operand = std::make_shared<SectionOperand>();
    operand->value = value->value;
    return operand;
}

std::shared_ptr<Instruction> parse_instruction(Lexer& lexer)
{
    auto instruction = std::make_shared<Instruction>();

    // Mnemonic
    auto mnemonic = lexer.consume_token_of_type(Token::Name);
    if(!mnemonic)
        return nullptr;
    instruction->mnemonic = mnemonic->value;

    // Maybe no arguments...
    auto newline = lexer.peek();
    if(newline && newline->type == Token::NewLine)
        return instruction;

    // Arguments
    while(true)
    {
        auto operand = parse_operand(lexer);
        if(!operand)
            return nullptr;

        instruction->members.push_back(operand);

        auto comma = lexer.consume_token_of_type(Token::Comma);
        if(!comma)
        {
            if(!ignore_at_least_one_of_type(lexer, Token::NewLine))
                PARSE_ERROR(lexer, "expected newline");
            break;
        }
    }
    return instruction;
}

std::shared_ptr<Directive> parse_directive(Lexer& lexer)
{
    auto dot = lexer.consume_token_of_type(Token::Dot);
    if(!dot)
        return nullptr;

    // FIXME: Avoid duplicating code
    auto directive = std::make_shared<Directive>();

    // Mnemonic
    auto mnemonic = lexer.consume_token_of_type(Token::Name);
    if(!mnemonic)
        return nullptr;
    directive->mnemonic = mnemonic->value;

    // Maybe no arguments...
    auto newline = lexer.peek();
    if(newline && newline->type == Token::NewLine)
        return directive;

    // Arguments
    while(true)
    {
        auto operand = parse_operand(lexer);
        if(!operand)
            return nullptr;

        directive->members.push_back(operand);

        auto comma = lexer.consume_token_of_type(Token::Comma);
        if(!comma)
        {
            if(!ignore_at_least_one_of_type(lexer, Token::NewLine))
                PARSE_ERROR(lexer, "expected newline");
            break;
        }
    }
    return directive;
}

std::shared_ptr<Assignment> parse_assignment(Lexer& lexer)
{
    std::shared_ptr<Assignment> assignment = std::make_shared<Assignment>();
    assignment->identifier = parse_identifier(lexer);
    if(!assignment->identifier)
        return nullptr;

    auto assignment_op = lexer.consume_token_of_type(Token::Assignment);
    if(!assignment_op)
        return nullptr;

    assignment->operand = parse_operand(lexer);
    if(!assignment->operand)
        PARSE_ERROR(lexer, "invalid operand in assignment");

    if(!ignore_at_least_one_of_type(lexer, Token::NewLine))
        PARSE_ERROR(lexer, "expected newline");

    return assignment;
}

std::shared_ptr<Label> parse_label(Lexer& lexer)
{
    std::shared_ptr<Label> label = std::make_shared<Label>();
    auto name = lexer.consume_token_of_type(Token::Name);
    if(!name)
        return nullptr;

    auto colon = lexer.consume_token_of_type(Token::Colon);
    if(!colon)
        return nullptr;

    label->name = name->value;
    return label;
}

std::shared_ptr<Operation> parse_operation(Lexer& lexer)
{
    std::shared_ptr<Operation> operation;
    size_t pos = lexer.index();
    operation = parse_assignment(lexer);
    if(!operation)
    {
        lexer.set_index(pos);
        operation = parse_directive(lexer);
        if(!operation)
        {
            lexer.set_index(pos);
            operation = parse_label(lexer);
            if(!operation)
            {
                lexer.set_index(pos);
                operation = parse_instruction(lexer);
                if(!operation)
                {
                    lexer.set_index(pos);
                    return nullptr;
                }
            }
        }
    }
    return operation;
}

std::shared_ptr<Block> parse_block(Lexer& lexer)
{
    ignore_all_of_type(lexer, Token::NewLine);

    std::shared_ptr<Block> block = std::make_shared<Block>();
    while(lexer.peek())
    {
        auto operation = parse_operation(lexer);
        if(!operation)
            PARSE_ERROR(lexer, "invalid operation");
        block->members.push_back(operation);
        ignore_all_of_type(lexer, Token::NewLine);
    }
    return block;
}

}
