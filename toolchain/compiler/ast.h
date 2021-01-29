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
    virtual bool from_lex(LexOutput& output);

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

// expression ::= [TODO]
class Expression : public Node
{
public:
    virtual bool from_lex(LexOutput& output) { return false; }

    virtual void display() const override
    {
        std::cout << "Expression" << std::endl;
    }
};

// function-body ::= { [expression ;]... }
class FunctionBody : public NodeSeq<Expression>
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

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "PointerTypeSpecifer: " << std::endl;
        type_specifier->display();
    }
};

// arg-definition ::= type-specifier name [= expression] [,]
class ArgDefinition : public Node
{
public:
    std::shared_ptr<TypeSpecifier> type;
    std::string name;
    //std::shared_ptr<Expression> default_value;
    std::string default_value;
    bool is_last = false;

    virtual bool from_lex(LexOutput& output);

    virtual void display() const override
    {
        std::cout << "ArgDefinition: " << std::endl;
        type->display();
        std::cout << name << " = " << default_value << std::endl;
    }
};

// function-definition ::= type-specifier name ( [arg-definition,]... ) function-body
class FunctionDefinition : public Declaration
{
public:
    std::shared_ptr<TypeSpecifier> type;
    std::string name;
    std::vector<std::shared_ptr<ArgDefinition>> args;
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
