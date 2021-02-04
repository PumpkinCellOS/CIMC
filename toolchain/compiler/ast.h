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

class Node
{
public:
    virtual bool from_lex(LexOutput& output) = 0;
    virtual void display() const = 0;
};

template<class T>
class NodeSeq : public Node
{
public:
    std::vector<std::shared_ptr<T>> subnodes;

    virtual void display() const override
    {
        std::cout << "<NodeSeq>:" << std::endl;
        for(auto it : subnodes)
        {
            it->display();
        }
    }
};

// declaration ::= function-definition
class Declaration : public Node
{
public:
    virtual void display() const override
    {
        std::cout << "Declaration" << std::endl;
    }
};

// translation-unit ::= declaration ...
class TranslationUnit : public NodeSeq<Declaration>
{
public:
    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "TranslationUnit:" << std::endl;
        NodeSeq::display();
    }
};

// expression ::= literal
class Expression : public Node
{
public:
    virtual bool from_lex(LexOutput& output) { return false; }

    virtual void display() const override
    {
        std::cout << "Expression" << std::endl;
    }
};

class Literal : public Expression
{
public:
    virtual bool from_lex(LexOutput& output) { return false; }

    virtual void display() const override
    {
        std::cout << "Literal" << std::endl;
    }
};

class NumericLiteral : public Expression
{
public:
    virtual bool from_lex(LexOutput& output) { return false; }

    virtual void display() const override
    {
        std::cout << "NumericLiteral" << std::endl;
    }
};

class IntegerLiteral : public Expression
{
public:
    int value = 0;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "IntegerLiteral: " << value << std::endl;
    }
};

class FunctionCall : public Expression
{
public:
    std::string function_name;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "FunctionCall: " << function_name << std::endl;
    }
};

// statement ::= [expression | return-statement];
class Statement : public Node
{
public:
    virtual bool from_lex(LexOutput& output) { return false; }
    virtual bool need_semicolon() const { return true; }

    virtual void display() const override
    {
        std::cout << "Statement" << std::endl;
    }
};

class ReturnStatement : public Statement
{
public:
    std::shared_ptr<Expression> expression;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "ReturnStatement: " << std::endl;
        if(expression)
            expression->display();
    }
};

class ExpressionStatement : public Statement
{
public:
    std::shared_ptr<Expression> expression;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "ExpressionStatement: " << std::endl;
        if(expression)
            expression->display();
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

    virtual void display() const override
    {
        std::cout << "FunctionBody:" << std::endl;
        NodeSeq::display();
    }
};

// type-specifier ::= simple-type-specifier | pointer-type-specifier
class TypeSpecifier : public Node
{
public:
    std::string name;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "TypeSpecifier: " << name << std::endl;
    }
};

class SimpleTypeSpecifier : public TypeSpecifier
{
public:
    enum SimpleType
    {
        Int, UnsignedInt, Short, UnsignedShort, Long, UnsignedLong, Bool, Char, Void, Unknown
    };
    SimpleType simple_type = Unknown;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "SimpleTypeSpecifier: " << (int)simple_type << std::endl;
    }
};

// pointer-type-specifier ::= type-specifier*
class PointerTypeSpecifier : public TypeSpecifier
{
public:
    std::shared_ptr<TypeSpecifier> type_specifier;
    size_t level;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "PointerTypeSpecifer (" << level << ")" << std::endl;
        type_specifier->display();
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

    virtual void display() const override
    {
        std::cout << "VariableDeclaration: " << std::endl;
        type->display();
        std::cout << name;
        if(default_value)
        {
            std::cout << " =";
            default_value->display();
        }
    }
};

class DeclarationStatement : public Statement
{
public:
    std::shared_ptr<Declaration> declaration;

    virtual bool from_lex(LexOutput& output) override;

    virtual void display() const override
    {
        std::cout << "DeclarationStatement: " << std::endl;
        declaration->display();
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

    virtual void display() const override
    {
        std::cout << "FunctionDefinition: ";
        type->display();
        std::cout << name << " (" << std::endl;
        for(auto it : args)
        {
            it->display();
        }
        std::cout << ")" << std::endl;
        body->display();
    }
};

}

}
