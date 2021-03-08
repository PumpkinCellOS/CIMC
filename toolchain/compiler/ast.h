#pragma once

#include "lexer.h"

#include <iostream>
#include <istream>
#include <memory>
#include <string>

namespace cpp_compiler
{

namespace AST
{

std::string indent(size_t depth, std::string _fill = "|   ", std::string _lastfill = "|-- ");

class Node
{
public:
    virtual bool from_lex(LexOutput& output) = 0;
    virtual void display(size_t depth) const = 0;
};

template<class T>
class NodeSeq : public Node
{
public:
    std::vector<std::shared_ptr<T>> subnodes;

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "<NodeSeq>:" << std::endl;
        for(auto it : subnodes)
        {
            it->display(depth + 1);
        }
    }
};

// declaration ::= function-definition
class Declaration : public Node
{
public:
    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "Declaration" << std::endl;
    }
};

// translation-unit ::= declaration ...
class TranslationUnit : public NodeSeq<Declaration>
{
public:
    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "TranslationUnit:" << std::endl;
        NodeSeq::display(depth + 1);
    }
};

// expression ::= literal
class Expression : public Node
{
public:
    virtual bool from_lex(LexOutput& output) { return false; }

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "Expression" << std::endl;
    }
};

class IntegerLiteral : public Expression
{
public:
    int value = 0;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "IntegerLiteral: " << value << std::endl;
    }
};

class Identifier : public Expression
{
public:
    std::string name;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "Identifier: " << name << std::endl;
    }
};

class FunctionCall : public Expression
{
public:
    std::string function_name;
    std::vector<std::shared_ptr<Expression>> arguments;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "FunctionCall: " << function_name << std::endl;
        std::cout << indent(depth) << "Arguments:" << std::endl;
        for(auto it : arguments)
            it->display(depth + 1);
    }
};

class UnaryExpression : public Expression
{
public:
    enum class Type
    {
        Address, // &
        Dereference, // *
        Plus, // +
        Minus, // -
        BitNegate, // ~
        LogicNegate, // !
        Increment, // ++
        Decrement // --
        // TODO:
        // Cast // ( type-specifier )
    } type;

    std::shared_ptr<Expression> expression;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "UnaryExpression: " << (int)type << std::endl;
        expression->display(depth + 1);
    }
};

// statement ::= [expression | return-statement];
class Statement : public Node
{
public:
    virtual bool from_lex(LexOutput& output) { return false; }
    virtual bool need_semicolon() const { return true; }

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "Statement" << std::endl;
    }
};

class ReturnStatement : public Statement
{
public:
    std::shared_ptr<Expression> expression;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "ReturnStatement: " << std::endl;
        if(expression)
            expression->display(depth + 1);
    }
};

class ExpressionStatement : public Statement
{
public:
    std::shared_ptr<Expression> expression;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "ExpressionStatement: " << std::endl;
        if(expression)
            expression->display(depth + 1);
    }
};

// control-statement :// if, else, switch, for, ...
class ControlStatement : public Statement
{

};

// function-body ::= { [statement...] }
// statement ::= [expression | return-statement | control-statement];
class FunctionBody : public NodeSeq<Statement>
{
public:
    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "FunctionBody:" << std::endl;
        NodeSeq::display(depth + 1);
    }
};

// type-specifier ::= simple-type-specifier | pointer-type-specifier
class TypeSpecifier : public Node
{
public:
    std::string name;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "TypeSpecifier: " << name << std::endl;
    }
};

class SimpleTypeSpecifier : public TypeSpecifier
{
public:
    enum SimpleType
    {
        // NOTE: UnsignedChar is not supported by cx16 for now :(
        Int, UnsignedInt, Short, UnsignedShort, Long, UnsignedLong, Bool, Char, UnsignedChar, Void, Unknown
    };
    SimpleType simple_type = Unknown;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "SimpleTypeSpecifier: " << (int)simple_type << std::endl;
    }
};

// pointer-type-specifier ::= type-specifier*
class PointerTypeSpecifier : public TypeSpecifier
{
public:
    std::shared_ptr<TypeSpecifier> type_specifier;
    size_t level;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "PointerTypeSpecifer (" << level << ")" << std::endl;
        type_specifier->display(depth + 1);
    }
};

// variable-declaration ::= type-specifier name [= expression]
class VariableDeclaration : public Declaration
{
public:
    std::shared_ptr<TypeSpecifier> type;
    std::string name;
    std::shared_ptr<Expression> default_value;

    virtual bool from_lex(LexOutput& output) override;

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "VariableDeclaration: " << std::endl;
        type->display(depth + 1);
        std::cout << indent(depth + 1) << name;
        if(default_value)
        {
            std::cout << " = " << std::endl;
            default_value->display(depth + 2);
        }
        else
            std::cout << std::endl;
    }
};

class DeclarationStatement : public Statement
{
public:
    std::shared_ptr<Declaration> declaration;

    virtual bool from_lex(LexOutput& output) override;

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "DeclarationStatement: " << std::endl;
        declaration->display(depth + 1);
    }
};

// function-definition ::= type-specifier name ( [arg-definition,]... ) function-body
class FunctionDefinition : public Declaration
{
public:
    std::shared_ptr<TypeSpecifier> type;
    std::string name;
    std::vector<std::shared_ptr<VariableDeclaration>> args;
    std::shared_ptr<FunctionBody> body;

    virtual bool from_lex(LexOutput& output);

    virtual void display(size_t depth) const override
    {
        std::cout << indent(depth) << "FunctionDefinition: " << std::endl;
        type->display(depth + 1);
        std::cout << indent(depth + 1) << name << std::endl;
        for(auto it : args)
        {
            it->display(depth + 1);
        }
        body->display(depth + 1);
    }
};

}

}
