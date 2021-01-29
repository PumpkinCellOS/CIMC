#include "ast.h"

namespace cpp_compiler
{

namespace AST
{

// translation-unit ::= declaration ...
bool TranslationUnit::from_lex(LexOutput& output)
{
    while(output.peek())
    {
        std::shared_ptr<Declaration> declaration;

        if(
           (declaration = std::make_shared<FunctionDefinition>())->from_lex(output)
        )
        {
            subnodes.push_back(declaration);
            return true;
        }

        PARSE_ERROR(output, "expected declaration");
    }
    return true;
}

// declaration ::= function-definition
bool Declaration::from_lex(LexOutput& output)
{
    auto function_definition = std::make_shared<FunctionDefinition>();
    if(!function_definition->from_lex(output))
        PARSE_ERROR(output, "expected function-definition");
    return true;
}

bool SimpleTypeSpecifier::from_lex(LexOutput& output)
{
    auto token = output.consume_token_of_type(Token::TypeName);
    if(!token)
        PARSE_ERROR(output, "expected type name");

    if(token->value == "int")
        simple_type = SimpleType::Int;
    else if(token->value == "char")
        simple_type = SimpleType::Char;
    else
        PARSE_ERROR(output, "invalid simple-type-specifier");
    return true;
}

std::shared_ptr<TypeSpecifier> parse_type_specifier(LexOutput& output, bool allow_pointer = true)
{
    std::shared_ptr<TypeSpecifier> type_specifier = std::make_shared<SimpleTypeSpecifier>();
    if(type_specifier->from_lex(output))
    {
        auto op = output.peek();
        if(op->type == Token::Operator && op->value == "*")
        {
            output.consume_token();
            auto ptr_type_specifier = std::make_shared<PointerTypeSpecifier>();
            ptr_type_specifier->type_specifier = type_specifier;
            return ptr_type_specifier;
        }
        return type_specifier;
    }
    return nullptr;
}

bool PointerTypeSpecifier::from_lex(LexOutput& output)
{
    return true;
}

bool FunctionBody::from_lex(LexOutput& output)
{
    // Opening
    auto lb = output.consume_token_of_type(Token::LeftCurly);
    if(!lb)
        return false;

    // Instructions
    // TODO

    // Closing
    auto rb = output.consume_token_of_type(Token::RightCurly);
    if(!rb)
        return false;
    return true;
}

bool TypeSpecifier::from_lex(LexOutput& output)
{
    auto token = output.consume_token_of_type(Token::TypeName);
    if(!token)
        PARSE_ERROR(output, "expected type name");
    name = token->value;
    return true;
}

// function-definition ::= type-specifier name ( [arg-definition,]... ) function-body
bool FunctionDefinition::from_lex(LexOutput& output)
{
    // Type
    type = parse_type_specifier(output);
    if(!type)
        PARSE_ERROR(output, "expected type-specifier in function-definition");

    // Name
    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
    {
        PARSE_ERROR(output, "expected function name");
    }
    name = name_token->value;

    // Arguments open
    if(!output.consume_token_of_type(Token::LeftBracket))
    {
        PARSE_ERROR(output, "expected '(' in function-definition");
    }

    // Arguments
    while(true)
    {
        auto argument = std::make_shared<ArgDefinition>();
        if(!argument->from_lex(output))
            PARSE_ERROR(output, "expected argument");
        args.push_back(argument);
        if(argument->is_last)
            break;
    }

    // Arguments close
    if(!output.consume_token_of_type(Token::RightBracket))
    {
        PARSE_ERROR(output, "expected ')' in function-definition");
    }

    // Function body
    auto function_body = std::make_shared<FunctionBody>();
    if(!function_body->from_lex(output))
        PARSE_ERROR(output, "expected function-body");
    body = function_body;

    return true;
}

// arg-definition ::= type-specifier name [= expression] [,]
bool ArgDefinition::from_lex(LexOutput& output)
{
    type = parse_type_specifier(output);
    if(!type)
        PARSE_ERROR(output, "expected type-specifier in arg-definition");

    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
    {
        PARSE_ERROR(output, "expected name in arg-definition");
    }
    name = name_token->value;

    auto default_value_eq = output.consume_token_of_type(Token::Operator);
    if(default_value_eq)
    {
        // TODO: Parse expressions
        auto default_value_token = output.consume_token_of_types({Token::Number, Token::String});
        if(!default_value_token)
        {
            PARSE_ERROR(output, "expected expression after '=' in arg-definition");
        }
        default_value = default_value_token->value;
    }

    auto comma = output.consume_token_of_type(Token::Comma);
    if(!comma)
    {
        is_last = true;
    }
    return true;
}

}

}
