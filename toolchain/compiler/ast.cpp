#include "ast.h"

namespace cpp_compiler
{

namespace AST
{

std::string indent(size_t depth, std::string _fill, std::string _lastfill)
{
    std::string str;
    for(size_t s = 0; s < depth; s++)
    {
        if(s == depth - 1)
            str += _lastfill;
        else
            str += _fill;
    }
    return str;
}

std::shared_ptr<Declaration> parse_declaration(LexOutput& output)
{
    std::shared_ptr<Declaration> declaration;

    size_t position = output.index();
    declaration = std::make_shared<FunctionDefinition>();
    if(!declaration->from_lex(output))
    {
        output.set_index(position);
        declaration = std::make_shared<VariableDeclaration>();
        if(!declaration->from_lex(output))
        {
            return nullptr;
        }
    }
    return declaration;
}

// translation-unit ::= [declaration ... ]
bool TranslationUnit::from_lex(LexOutput& output)
{
    while(output.peek())
    {
        std::shared_ptr<Declaration> declaration = parse_declaration(output);
        if(declaration)
        {
            subnodes.push_back(declaration);
            continue;
        }

        PARSE_ERROR(output, "expected declaration");
    }
    return true;
}

bool SimpleTypeSpecifier::from_lex(LexOutput& output)
{
    auto token = output.consume_token_of_type(Token::TypeName);
    if(!token)
        return false;

    if(token->value == "int")
        simple_type = SimpleType::Int;
    else if(token->value == "char")
        simple_type = SimpleType::Char;
    else if(token->value == "void")
        simple_type = SimpleType::Void;
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
    size_t position = output.index();
    if(!statement->from_lex(output))
    {
        output.set_index(position);
        statement = std::make_shared<ExpressionStatement>();
        if(!statement->from_lex(output))
        {
            output.set_index(position);
            statement = std::make_shared<DeclarationStatement>();
            if(!statement->from_lex(output))
            {
                return nullptr;
            }
        }
    }

    // Semicolon (if applicable)
    if(!statement->need_semicolon())
        return statement;

    auto semicolon = output.consume_token_of_type(Token::Semicolon);
    if(semicolon)
        return statement;

    PARSE_ERROR(output, "expected ';' in statement");
}

// expression ::= comma-expression | ( expression ) | function-call | numeric-literal | identifier
// TODO: operators
std::shared_ptr<Expression> parse_expression(LexOutput& output)
{
    // TODO: Make FunctionCall a postfix-expression
    std::shared_ptr<Expression> expression;
    expression = std::make_shared<FunctionCall>();
    size_t position = output.index();
    if(!expression->from_lex(output))
    {
        output.set_index(position);
        expression = std::make_shared<UnaryExpression>();
        if(!expression->from_lex(output))
        {
            output.set_index(position);
            expression = std::make_shared<IntegerLiteral>();
            if(!expression->from_lex(output))
            {
                output.set_index(position);
                expression = std::make_shared<Identifier>();
                if(!expression->from_lex(output))
                {
                    output.set_index(position);
                    return nullptr;
                }
            }
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

bool Identifier::from_lex(LexOutput& output)
{
    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
        return false;

    name = name_token->value;
    return true;
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
            return false;

        // Arguments
        bool is_first = true;
        while(true)
        {
            auto expression = parse_expression(output);
            if(!expression)
            {
                if(is_first)
                    break;
                else
                    PARSE_ERROR(output, "expected expression in function call");
            }

            arguments.push_back(expression);

            auto comma = output.consume_token_of_type(Token::Comma);
            if(!comma)
                break;

            is_first = false;
        }

        auto rc = output.consume_token_of_type(Token::RightBracket);
        if(!rc)
            PARSE_ERROR(output, "expected ')'");

        return true;
    }
    return false;
}

// unary-expression ::= unary-operator expression
bool UnaryExpression::from_lex(LexOutput& output)
{
    // Operator
    auto oper = output.consume_token_of_type(Token::Operator);
    if(!oper)
        return false;

    /*
        Address, // &
        Dereference, // *
        Plus, // +
        Minus, // -
        BitNegate, // ~
        LogicNegate, // !
        Increment, // ++
        Decrement // --
    */

    if(oper->value == "&")
        type = Type::Address;
    else if(oper->value == "*")
        type = Type::Dereference;
    else if(oper->value == "+")
        type = Type::Plus;
    else if(oper->value == "-")
        type = Type::Minus;
    else if(oper->value == "~")
        type = Type::BitNegate;
    else if(oper->value == "!")
        type = Type::LogicNegate;
    else if(oper->value == "++")
        type = Type::Increment;
    else if(oper->value == "--")
        type = Type::Decrement;

    // TODO: Casts

    // Expression
    expression = parse_expression(output);
    if(!expression)
        return false;

    return true;
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
            PARSE_ERROR(output, "expected statement");
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
    if(!expression)
        return false;
    return true;
}

bool TypeSpecifier::from_lex(LexOutput& output)
{
    auto token = output.consume_token_of_type(Token::TypeName);
    if(!token)
        return false;
    name = token->value;
    return true;
}

// function-definition ::= type-specifier name ( [arg-definition,]... ) function-body
bool FunctionDefinition::from_lex(LexOutput& output)
{
    // Type
    type = parse_type_specifier(output);
    if(!type)
        return false;

    // Name
    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
    {
        PARSE_ERROR(output, "expected function name");
    }
    name = name_token->value;

    // Arguments open
    if(!output.consume_token_of_type(Token::LeftBracket))
        return false;

    // Arguments
    while(true)
    {
        auto argument = std::make_shared<VariableDeclaration>();
        if(!argument->from_lex(output))
            break;
        args.push_back(argument);

        // Comma
        auto comma = output.consume_token_of_type(Token::Comma);
        if(!comma)
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
        PARSE_ERROR(output, "expected function body");
    body = function_body;

    return true;
}

// variable-declaration ::= type-specifier name [= expression]
bool VariableDeclaration::from_lex(LexOutput& output)
{
    type = parse_type_specifier(output);
    if(!type)
        return false;

    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
    {
        PARSE_ERROR(output, "expected name in variable declaration");
    }
    name = name_token->value;

    auto default_value_eq = output.consume_token_of_type(Token::Operator);
    if(default_value_eq)
    {
        default_value = parse_expression(output);
        if(!default_value)
            PARSE_ERROR(output, "expected expression in initializer");
    }

    return true;
}

bool DeclarationStatement::from_lex(LexOutput& output)
{
    auto decl = parse_declaration(output);
    if(!decl)
        return false;
    declaration = decl;
    return true;
}

}

}
