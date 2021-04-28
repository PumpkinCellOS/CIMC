#pragma once

#include "lexer.h"
#include "../cpp/parser.h"

#include <vector>
#include <string>
#include <memory>

namespace assembler
{

class Node
{
public:
    virtual std::string display() const { return name(); }
    virtual std::string display_block(size_t depth) const { return name() + "\n"; }
    virtual bool is_block_node() const { return false; }
    virtual std::string name() const { return "<InvalidNode>"; }
};

template<class Member>
class BlockNode : public Node
{
public:
    std::vector<std::shared_ptr<Member>> members;
    virtual bool is_block_node() const override { return true; }

    virtual std::string display_block(size_t depth) const override
    {
        std::string out = cpp_compiler::AST::indent(depth) + display() + "\n";
        for(auto& it: members)
        {
            if(it->is_block_node())
                out += it->display_block(depth + 1);
            else
                out += cpp_compiler::AST::indent(depth + 1) + it->display() + "\n";
        }
        return out;
    }
};

// operand ::= address-operand | section-operand | isr-operand | number | name
class Operand : public Node
{
public:
    enum class Type
    {
        DecNumber,
        HexNumber,
        Name,
        String,
        Invalid
    } type;

    std::string value;

    std::string type_to_string() const
    {
        switch(type)
        {
            case Type::DecNumber: return "DecNumber";
            case Type::HexNumber: return "HexNumber";
            case Type::Name: return "Name";
            case Type::String: return "String";
            case Type::Invalid: return "Invalid";
        }
        return "???";
    }

    virtual std::string display() const override
    {
        return "GenericOperand<" + type_to_string() + ">(" + value + ")";
    }
};

// identifier ::= [ name | relative-address ]
class Identifier : public Node
{
};

class NameIdentifier : public Identifier
{
public:
    std::string name;

    virtual std::string display() const override
    {
        return "NameIdentifier[" + name + "]";
    }
};

// relative-address ::= name [+-] number
class RelativeAddress : public Identifier
{
public:
    std::string name;
    int offset;

    virtual std::string display() const override
    {
        return "RelativeAddress[" + name + (offset >= 0 ? "+" : "") + std::to_string(offset) + "]";
    }
};

// address-operand ::= [ identifier ]
class AddressOperand : public Operand
{
public:
    std::shared_ptr<Identifier> identifier;

    virtual std::string display() const override
    {
        return "AddressOperand[" + identifier->display() + "]";
    }
};

// isr-operand ::= # name
class ISROperand : public Operand
{
public:
    virtual std::string display() const override
    {
        return "ISROperand[" + value + "]";
    }
};

class SectionOperand : public Operand
{
    virtual std::string display() const override
    {
        return "SectionOperand ." + value;
    }
};

class Operation : public BlockNode<Operand>
{
public:
    std::string mnemonic;
};

class Instruction : public Operation
{
public:
    virtual std::string display() const override
    {
        return "Instruction:" + mnemonic;
    }
};

// directive ::= . instruction
class Directive : public Operation
{
public:
    std::string mnemonic;

    virtual std::string display() const override
    {
        return "Directive:" + mnemonic;
    }
};

// assignment ::= identifier := operand
class Assignment : public Operation
{
public:
    std::shared_ptr<Identifier> identifier;
    std::shared_ptr<Operand> operand;

    virtual std::string display() const override
    {
        return "Assignment " + identifier->display() + " := " + operand->display();
    }
};

// label ::= name :
class Label : public Operation
{
public:
    std::string name;

    virtual std::string display() const override
    {
        return "Label " + name + ":";
    }
};

class Block : public BlockNode<Operation>
{
public:
    virtual std::string name() const override
    {
        return "Block";
    }
};

std::shared_ptr<Operand> parse_operand(Lexer& lexer);
std::shared_ptr<Identifier> parse_identifier(Lexer& lexer);
std::shared_ptr<RelativeAddress> parse_relative_address(Lexer& lexer);
std::shared_ptr<AddressOperand> parse_address_operand(Lexer& lexer);
std::shared_ptr<ISROperand> parse_isr_operand(Lexer& lexer);
std::shared_ptr<SectionOperand> parse_section_operand(Lexer& lexer);
std::shared_ptr<Instruction> parse_instruction(Lexer& lexer);
std::shared_ptr<Directive> parse_directive(Lexer& lexer);
std::shared_ptr<Assignment> parse_assignment(Lexer& lexer);
std::shared_ptr<Label> parse_label(Lexer& lexer);
std::shared_ptr<Operation> parse_operation(Lexer& lexer);
std::shared_ptr<Block> parse_block(Lexer& lexer);

}