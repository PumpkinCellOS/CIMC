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

std::shared_ptr<TypeSpecifier> parse_type_specifier(LexOutput& output)
{
    std::shared_ptr<TypeSpecifier> type_specifier = std::make_shared<SimpleTypeSpecifier>();
    if(type_specifier->from_lex(output))
    {
        auto op = output.peek();
        if(op->type == Token::Operator)
        {
            bool all_are_stars = true;
            for(auto ch : op->value)
            {
                if(ch != '*')
                {
                    all_are_stars = false;
                    break;
                }
            }

            if(!all_are_stars)
                PARSE_ERROR(output, "invalid pointer specifier");

            output.consume_token();
            auto ptr_type_specifier = std::make_shared<PointerTypeSpecifier>();
            ptr_type_specifier->type_specifier = type_specifier;
            ptr_type_specifier->level = op->value.size();
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

std::shared_ptr<Statement> parse_statement(LexOutput& output)
{
    // Statement
    std::shared_ptr<Statement> statement;
    statement = std::make_shared<ReturnStatement>();
    if(!statement->from_lex(output))
    {
        statement = std::make_shared<ExpressionStatement>();
        if(!statement->from_lex(output))
        {
            return nullptr;
        }
    }

    // Semicolon
    auto semicolon = output.peek();
    if(semicolon && semicolon->type == Token::Semicolon)
    {
        output.consume_token();
        return statement;
    }
    PARSE_ERROR(output, "expected ';' in statement");
}

// expression ::= numeric-literal | function-call
// TODO: operators
std::shared_ptr<Expression> parse_expression(LexOutput& output)
{
    std::shared_ptr<Expression> expression;
    expression = std::make_shared<IntegerLiteral>();
    if(!expression->from_lex(output))
    {
        expression = std::make_shared<FunctionCall>();
        if(!expression->from_lex(output))
        {
            return nullptr;
        }
    }
    return expression;
}

bool IntegerLiteral::from_lex(LexOutput& output)
{
    auto token = output.peek();
    if(token && token->type == Token::Number)
    {
        output.consume_token();
        try
        {
            value = std::stoi(token->value);
        }
        catch(...)
        {
            PARSE_ERROR(output, "invalid integer literal");
        }
        return true;
    }
    return false;
}

// function-call ::= function-name ( [expression,...] )
bool FunctionCall::from_lex(LexOutput& output)
{
    auto name = output.peek();
    if(name && name->type == Token::Name)
    {
        function_name = name->value;

        output.consume_token();
        auto lc = output.consume_token_of_type(Token::LeftBracket);
        if(!lc)
            PARSE_ERROR(output, "expected '('");

        // TODO: Arguments

        auto rc = output.consume_token_of_type(Token::RightBracket);
        if(!rc)
            PARSE_ERROR(output, "expected ')'");

        return true;
    }
    return false;
}

bool FunctionBody::from_lex(LexOutput& output)
{
    // Opening
    auto lb = output.consume_token_of_type(Token::LeftCurly);
    if(!lb)
        return false;

    // Instructions
    do
    {
        auto token = output.peek();
        if(!token || token->type == Token::RightCurly)
            break;

        auto statement = parse_statement(output);
        if(!statement)
            PARSE_ERROR(output, "expected statement or '}'");
        subnodes.push_back(statement);
    } while(true);

    // Closing
    auto rb = output.consume_token_of_type(Token::RightCurly);
    if(!rb)
        return false;
    return true;
}

bool ReturnStatement::from_lex(LexOutput& output)
{
    auto retkw = output.peek();
    if(retkw && retkw->type == Token::Keyword && retkw->value == "return")
    {
        output.consume_token();
        expression = parse_expression(output);
        return true;
    }
    return false;
}

bool ExpressionStatement::from_lex(LexOutput& output)
{
    expression = parse_expression(output);
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
