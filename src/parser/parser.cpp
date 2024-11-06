#include "parser.hpp"
#include "AST-node.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include "lexer/token.hpp"
#include "return.hpp"
#include "statement.hpp"
#include <algorithm>
#include <array>
#include <memory>
#include <stdexcept>
#include <vector>

namespace parsing
{

Token Parser::currentTok()
{
    return m_tokens[m_cur_tok];
}

void Parser::advanceTok()
{
    ++m_cur_tok;
}

void Parser::consumeNewlines()
{
    while (m_tokens[m_cur_tok].m_id == TOKEN_NEWLINE)
    {
        m_cur_tok++;
    }
}

bool Parser::isCurrentRoutineCall()
{
    return peekNextToken().m_id == TOKEN_LPAREN;
}

Token Parser::peekNextToken()
{
    return m_tokens[m_cur_tok + 1];
}

std::shared_ptr<RoutineParameter> Parser::parse_routine_parameter()
{
    auto type = currentTok().m_id;
    if (type != TOKEN_IDENTIFIER && type != TOKEN_INTEGER && type != TOKEN_BOOLEAN && type != TOKEN_REAL)
    {
        throw std::runtime_error("Identifier expected or primitive type expected");
    }
    auto type_identifier = currentTok().m_value;
    advanceTok();

    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("Identifier expected");
    }
    auto var_name = currentTok().m_value;
    advanceTok();

    return std::make_shared<RoutineParameter>(var_name, type_identifier);
}

std::shared_ptr<Routine> Parser::parse_routine_decl()
{
    advanceTok();
    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("Identifier was expected !");
    }
    auto res = std::make_shared<Routine>(currentTok().m_value);

    advanceTok();
    if (currentTok().m_id != TOKEN_LPAREN)
    {
        throw std::runtime_error("'(' expected");
    }

    // they can only be in form of [primitive_type | identifier_of_type] identifier
    advanceTok();
    while (true)
    {
        if (currentTok().m_id == TOKEN_RPAREN)
        {
            break;
        }
        res->m_params.push_back(parse_routine_parameter());
        if (currentTok().m_id == TOKEN_COMA)
        {
            advanceTok();
        }
    }

    if (currentTok().m_id != TOKEN_RPAREN)
    {
        throw std::runtime_error("')' expected");
    }
    advanceTok();

    if (currentTok().m_id == TOKEN_ARROW)
    {
        advanceTok();
        auto type = currentTok().m_id;
        if (type != TOKEN_IDENTIFIER && type != TOKEN_INTEGER && type != TOKEN_BOOLEAN && type != TOKEN_REAL)
        {
            throw std::runtime_error("Identifier expected or primitive type expected");
        }
        res->return_type = currentTok().m_value;
        advanceTok();
    }

    if (currentTok().m_id != TOKEN_IS)
    {
        throw std::runtime_error("'is' expected");
    }

    advanceTok();

    res->m_body = parse_body();

    if (currentTok().m_id != TOKEN_END)
    {
        throw std::runtime_error("end expected after routine !");
    }
    advanceTok();

    return res;
}

// Is related to expressions
namespace expressions
{

std::shared_ptr<Expression>
expression_parser_builder(std::vector<std::shared_ptr<Expression>> expression_line, bool return_zero);

template <typename T>
std::vector<T> take_subvector(const std::vector<T>& vec, int begin, int end)
{
    if (begin > end || end > vec.size())
    {
        throw std::out_of_range("Invalid range for sub vector");
    }

    std::vector<T> sub_vec(vec.begin() + begin, vec.begin() + end);
    return sub_vec;
}

using ExpressionParserBuilderPtr = std::shared_ptr<Expression> (*)(std::vector<std::shared_ptr<Expression>>, bool);

std::shared_ptr<Math> get_fork_in_expression(
    const std::vector<std::shared_ptr<Expression>>& expression_line,
    std::vector<GrammarUnit>& units,
    std::shared_ptr<Math>& fork,
    bool& achieved)
{
    for (auto unit : units)
    {
        if (expression_line[0]->get_grammar() == unit)
        {
            // already an expression
            const auto& check_fork = std::dynamic_pointer_cast<Math>(expression_line[0]);

            if (check_fork->m_right != nullptr || check_fork->m_left != nullptr)
            {
                break;
            }

            // case like -5 + 6 ... or +3 + 7
            if (unit == GrammarUnit::PLUS || unit == GrammarUnit::MINUS)
            {
                break;
            }

            throw std::runtime_error(
                "Unexpected item at the beginning of an expression: " + expression_line[0]->gr_to_str());
        }
    }

    bool need_to_break = false;

    for (int i = 0; i < expression_line.size(); ++i)
    {
        if (need_to_break)
        {
            break;
        }

        const auto& item = expression_line[i];

        for (auto& unit : units)
        {
            if (item->get_grammar() == unit)
            {
                const auto& check_fork = std::dynamic_pointer_cast<Math>(item);

                if (check_fork->m_right != nullptr || check_fork->m_left != nullptr)
                {
                    break;
                }


                achieved = true;

                switch (unit)
                {
                    case GrammarUnit::PLUS:
                        fork = std::make_shared<Plus>();
                        break;
                    case GrammarUnit::MINUS:
                        fork = std::make_shared<Minus>();
                        break;
                    case GrammarUnit::MULTIPLICATE:
                        fork = std::make_shared<Multiplication>();
                        break;
                    case GrammarUnit::DIVISION:
                        fork = std::make_shared<Division>();
                        break;
                    case GrammarUnit::MOD:
                        fork = std::make_shared<Mod>();
                        break;
                    case GrammarUnit::GREATER:
                        fork = std::make_shared<Greater>();
                        break;
                    case GrammarUnit::LESS:
                        fork = std::make_shared<Less>();
                        break;
                    case GrammarUnit::GREATER_EQUAL:
                        fork = std::make_shared<GreaterEqual>();
                        break;
                    case GrammarUnit::LESS_EQUAL:
                        fork = std::make_shared<LessEqual>();
                        break;
                    case GrammarUnit::EQUAL:
                        fork = std::make_shared<Equal>();
                        break;
                    case GrammarUnit::NOT_EQUAL:
                        fork = std::make_shared<NotEqual>();
                        break;
                    case GrammarUnit::AND:
                        fork = std::make_shared<And>();
                        break;
                    case GrammarUnit::OR:
                        fork = std::make_shared<Or>();
                        break;
                    case GrammarUnit::XOR:
                        fork = std::make_shared<Xor>();
                        break;
                    default:
                        break;
                }

                auto left_part = take_subvector(expression_line, 0, i);

                // case -8 + 3 or +5 - 2 (valid)
                if (left_part.empty() && (unit == GrammarUnit::PLUS || unit == GrammarUnit::MINUS))
                {
                    // case --8 + 3 (invalid)
                    if (expression_line[i + 1]->get_grammar() == GrammarUnit::PLUS
                        || expression_line[i + 1]->get_grammar() == GrammarUnit::MINUS)
                    {
                        throw std::runtime_error(
                            "Unexpected token in a expression: " + expression_line[i + 1]->gr_to_str());
                    }

                    fork->m_left = expression_parser_builder(left_part, true);
                }
                else
                {
                    fork->m_left = expression_parser_builder(left_part, false);
                }


                if (i == expression_line.size() - 1)
                {
                    throw std::runtime_error("This item cannot be last term in expression");
                }

                int next_pos = expression_line.size();
                auto right_part = take_subvector(expression_line, i + 1, next_pos);
                fork->m_right = expression_parser_builder(right_part, false);

                need_to_break = true;
                break;
            }
        }
    }

    return fork;
}

// A recursion for building a tree
std::shared_ptr<Expression>
expression_parser_builder(std::vector<std::shared_ptr<Expression>> expression_line, bool return_zero)
{
    // Order: */ +- <> = xor and or (start from the end)

    if (return_zero)
    {
        return std::make_shared<Integer>(0);
    }

    std::shared_ptr<Math> fork;
    std::vector<GrammarUnit> grammar_units;
    bool was_achieved = false;

    // or
    grammar_units.push_back(GrammarUnit::OR);
    fork = get_fork_in_expression(expression_line, grammar_units, fork, was_achieved);
    if (was_achieved)
    {
        return fork;
    }
    grammar_units.clear();

    // and
    grammar_units.push_back(GrammarUnit::AND);
    fork = get_fork_in_expression(expression_line, grammar_units, fork, was_achieved);
    if (was_achieved)
    {
        return fork;
    }
    grammar_units.clear();

    // xor
    grammar_units.push_back(GrammarUnit::XOR);
    fork = get_fork_in_expression(expression_line, grammar_units, fork, was_achieved);
    if (was_achieved)
    {
        return fork;
    }
    grammar_units.clear();

    // = /=
    grammar_units.push_back(GrammarUnit::EQUAL);
    grammar_units.push_back(GrammarUnit::NOT_EQUAL);
    fork = get_fork_in_expression(expression_line, grammar_units, fork, was_achieved);
    if (was_achieved)
    {
        return fork;
    }
    grammar_units.clear();

    // > < <= >= = /=
    grammar_units.push_back(GrammarUnit::GREATER);
    grammar_units.push_back(GrammarUnit::LESS);
    grammar_units.push_back(GrammarUnit::LESS_EQUAL);
    grammar_units.push_back(GrammarUnit::GREATER_EQUAL);
    grammar_units.push_back(GrammarUnit::EQUAL);
    grammar_units.push_back(GrammarUnit::NOT_EQUAL);
    fork = get_fork_in_expression(expression_line, grammar_units, fork, was_achieved);
    if (was_achieved)
    {
        return fork;
    }
    grammar_units.clear();

    // + -
    grammar_units.push_back(GrammarUnit::PLUS);
    grammar_units.push_back(GrammarUnit::MINUS);
    fork = get_fork_in_expression(expression_line, grammar_units, fork, was_achieved);
    if (was_achieved)
    {
        return fork;
    }
    grammar_units.clear();

    // * / %
    grammar_units.push_back(GrammarUnit::MULTIPLICATE);
    grammar_units.push_back(GrammarUnit::DIVISION);
    grammar_units.push_back(GrammarUnit::MOD);
    fork = get_fork_in_expression(expression_line, grammar_units, fork, was_achieved);
    if (was_achieved)
    {
        return fork;
    }
    grammar_units.clear();

    // Now, here are only numbers and identifiers
    if (expression_line.size() != 1)
    {
        throw std::runtime_error("Two numbers/identifiers should be connected by some action (e.g. 5 2 -> 5 + 2)");
    }

    return expression_line[0];
}

bool is_end_of_expression(int token_id)
{
    // clang-format off
    std::array end_tokens {
        TOKEN_NEWLINE,
        TOKEN_THEN,
        TOKEN_DOTDOT,
        TOKEN_SEMICOLON,
        TOKEN_LOOP,
        TOKEN_RPAREN,
        TOKEN_RBRACKET,
        TOKEN_COMA
    };
    // clang-format on

    return std::any_of(end_tokens.begin(), end_tokens.end(), [token_id](int val) { return val == token_id; });
}

} // namespace expressions

std::shared_ptr<Expression> Parser::parse_expression()
{
    std::vector<std::shared_ptr<Expression>> expression_line;

    while (!expressions::is_end_of_expression(currentTok().m_id))
    {
        std::shared_ptr<Expression> item_to_add = nullptr;

        bool do_not_take_token = false;

        switch (currentTok().m_id)
        {
            case TOKEN_IDENTIFIER: {
                do_not_take_token = true;

                if (peekNextToken().m_id == TOKEN_LPAREN)
                {
                    auto routine_call_res = std::make_shared<RoutineCallResult>();
                    routine_call_res->m_routine_call = parse_routine_call();

                    item_to_add = routine_call_res;
                    break;
                }

                item_to_add = parse_modifiable_primary();
                break;
            }
            case TOKEN_CONST_INT: {
                int value = std::stoi(currentTok().m_value);
                item_to_add = std::make_shared<Integer>(value);
                break;
            }
            case TOKEN_CONST_REAL: {
                double value = std::stod(currentTok().m_value);
                item_to_add = std::make_shared<Real>(value);
                break;
            }
            case TOKEN_PLUS:
                item_to_add = std::make_shared<Plus>();
                break;
            case TOKEN_MINUS:
                item_to_add = std::make_shared<Minus>();
                break;
            case TOKEN_MULTIP:
                item_to_add = std::make_shared<Multiplication>();
                break;
            case TOKEN_DIVISION:
                item_to_add = std::make_shared<Division>();
                break;
            case TOKEN_GREATER:
                item_to_add = std::make_shared<Greater>();
                break;
            case TOKEN_LESS:
                item_to_add = std::make_shared<Less>();
                break;
            case TOKEN_GREATEREQ:
                item_to_add = std::make_shared<GreaterEqual>();
                break;
            case TOKEN_LESSEQ:
                item_to_add = std::make_shared<LessEqual>();
                break;
            case TOKEN_EQUAL:
                item_to_add = std::make_shared<Equal>();
                break;
            case TOKEN_NOTEQUAL:
                item_to_add = std::make_shared<NotEqual>();
                break;
            case TOKEN_MOD:
                item_to_add = std::make_shared<Mod>();
                break;
            case TOKEN_AND:
                item_to_add = std::make_shared<And>();
                break;
            case TOKEN_OR:
                item_to_add = std::make_shared<Or>();
                break;
            case TOKEN_XOR:
                item_to_add = std::make_shared<Xor>();
                break;
            case TOKEN_TRUE:
                item_to_add = std::make_shared<True>();
                break;
            case TOKEN_FALSE:
                item_to_add = std::make_shared<False>();
                break;

            // More complex structures
            case TOKEN_LPAREN: {
                advanceTok();
                auto nested_expression = parse_expression();

                item_to_add = nested_expression;
                break;
            }
            case TOKEN_RPAREN:
            case TOKEN_RBRACKET:
                advanceTok();
                break;

            default:
                throw std::runtime_error("Some item in expression was not recognised: " + currentTok().m_value);
        }

        if (item_to_add == nullptr)
        {
            throw std::runtime_error("Some item in expression was not recognised: " + currentTok().m_value);
        }

        expression_line.push_back(item_to_add);
        if (!do_not_take_token)
        {
            advanceTok();
        }
    }

    // Building a tree based on a vector
    auto tree = expressions::expression_parser_builder(expression_line, false);
    return tree;
}

std::shared_ptr<Modifiable> Parser::parse_modifiable_primary()
{
    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("identifier expected !");
    }

    auto modif_primary = std::make_shared<Modifiable>(currentTok().m_value);
    advanceTok();
    while (true)
    {
        auto curtk = currentTok();
        if (curtk.m_id != TOKEN_DOT && curtk.m_id != TOKEN_LBRACKET)
        {
            break;
        }

        if (curtk.m_id == TOKEN_DOT)
        {
            advanceTok();
            auto field = std::make_shared<RecordAccess>();
            if (currentTok().m_id != TOKEN_IDENTIFIER)
            {
                throw std::runtime_error("identifier expected !");
            }
            field->identifier = currentTok().m_value;
            modif_primary->m_chain.push_back(field);
            advanceTok();
        }
        else
        {
            // TOKEN_LBRACKET
            advanceTok();
            auto expr = parse_expression();

            if (currentTok().m_id != TOKEN_RBRACKET)
            {
                throw std::runtime_error("']' expected !");
            }

            auto array_acc = std::make_shared<ArrayAccess>();
            array_acc->access = expr;
            modif_primary->m_chain.push_back(array_acc);
            advanceTok();
        }
    }

    return modif_primary;
}

std::shared_ptr<Assignment> Parser::parse_assignment()
{
    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("no modifiable identifier");
    }
    auto assignment = std::make_shared<Assignment>();
    auto modifiable = parse_modifiable_primary();

    if (currentTok().m_id != TOKEN_ASSIGNMENT)
    {
        throw std::runtime_error("no assignment token found !");
    }

    advanceTok();
    auto expression = parse_expression();

    assignment->m_expression = expression;
    assignment->m_modifiable = modifiable;
    return assignment;
}

std::shared_ptr<RoutineCall> Parser::parse_routine_call()
{
    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("no routine identifier");
    }
    std::shared_ptr<RoutineCall> call = std::make_shared<RoutineCall>(currentTok().m_value);
    advanceTok();

    if (currentTok().m_id != TOKEN_LPAREN)
    {
        throw std::runtime_error("expected '(' ");
    }
    advanceTok();

    while (currentTok().m_id != TOKEN_RPAREN)
    {
        call->m_parameters.push_back(parse_expression());
        if (currentTok().m_id == TOKEN_COMA)
        {
            advanceTok();
        }
    }

    advanceTok();

    return call;
}

std::shared_ptr<Statement> Parser::parse_statement()
{
    switch (currentTok().m_id)
    {
        case TOKEN_IF:
            return parse_if_statement();
        case TOKEN_WHILE:
            return parse_while_statement();
        case TOKEN_FOR:
            return parse_for_statement();
        case TOKEN_IDENTIFIER:
            if (isCurrentRoutineCall())
            {
                return parse_routine_call();
            }
            else
            {
                return parse_assignment();
            }
        default:
            throw std::runtime_error("can't parse statement !");
    }
}

std::shared_ptr<Body> Parser::parse_body()
{
    auto body_res = std::make_shared<Body>();
    while (true)
    {
        consumeNewlines();
        switch (currentTok().m_id)
        {
            case TOKEN_VAR:
                body_res->m_items.push_back(parse_variable_decl());
                break;
            case TOKEN_TYPE:
                body_res->m_items.push_back(parse_type_decl());
                break;
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_FOR:
            case TOKEN_IDENTIFIER:
                body_res->m_items.push_back(parse_statement());
                break;
            case TOKEN_ELSE:
            case TOKEN_END:
                break;
            case TOKEN_RETURN: {
                advanceTok();
                auto expr = parse_expression();
                auto returns = std::make_shared<ReturnStatement>(expr);
                body_res->m_items.push_back(returns);
            }
            break;
            default:
                throw std::runtime_error("Can't parse body !");
        }
        if (currentTok().m_id == TOKEN_NEWLINE || currentTok().m_id == TOKEN_SEMICOLON)
        {
            advanceTok();
        }
        if (currentTok().m_id == TOKEN_ELSE)
        {
            break;
        }
        if (currentTok().m_id == TOKEN_END)
        {
            break;
        }
    }
    consumeNewlines();
    return body_res;
}

std::shared_ptr<RecordType> Parser::parse_record_decl(std::string name)
{
    if (currentTok().m_id != TOKEN_RECORD)
    {
        throw std::runtime_error("'record' token is expected!");
    }
    advanceTok();

    auto record = std::make_shared<RecordType>(name);
    while (currentTok().m_id != TOKEN_END)
    {
        consumeNewlines();
        record->m_fields.push_back(parse_variable_decl());
        consumeNewlines();
    }

    advanceTok();
    return record;
}

std::shared_ptr<ArrayType> Parser::parse_array_type()
{
    if (currentTok().m_id != TOKEN_ARRAY)
    {
        throw std::runtime_error("'array' token is expected!");
    }

    advanceTok();
    if (currentTok().m_id != TOKEN_LBRACKET)
    {
        throw std::runtime_error("'[' is expected !");
    }
    advanceTok();

    std::shared_ptr<Expression> number_of_elements = parse_expression();

    if (currentTok().m_id != TOKEN_RBRACKET)
    {
        throw std::runtime_error("']' is expected !");
    }

    advanceTok();

    if (currentTok().m_id != TOKEN_IDENTIFIER && currentTok().m_id != TOKEN_BOOLEAN
        && currentTok().m_id != TOKEN_INTEGER && currentTok().m_id != TOKEN_REAL && currentTok().m_id != TOKEN_ARRAY)
    {
        throw std::runtime_error("Expected a type of the array !");
    }

    std::shared_ptr<ArrayType> array_type;

    if (currentTok().m_id == TOKEN_ARRAY)
    {
        array_type = std::make_shared<ArrayType>(parse_array_type(), number_of_elements);
    }
    else
    {
        auto type = std::make_shared<PrimitiveType>(currentTok().m_value);
        array_type = std::make_shared<ArrayType>(type, number_of_elements);
    }

    advanceTok();

    return array_type;
}

std::shared_ptr<Variable> Parser::parse_variable_decl()
{
    if (currentTok().m_id != TOKEN_VAR)
    {
        throw std::runtime_error("var is expected to make a variable decl !");
    }
    advanceTok();

    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("identifier to declare a var name is expected !");
    }
    auto var_name = currentTok().m_value;

    advanceTok();

    if (currentTok().m_id != TOKEN_COLON)
    {
        throw std::runtime_error("colon expected !");
    }
    advanceTok();

    std::shared_ptr<Type> type;
    switch (currentTok().m_id)
    {
        case TOKEN_BOOLEAN:
        case TOKEN_INTEGER:
        case TOKEN_IDENTIFIER:
        case TOKEN_REAL:
            type = std::make_shared<PrimitiveType>(currentTok().m_value);
            advanceTok();
            if (currentTok().m_id == TOKEN_IS)
            {
                advanceTok();
                return make_shared<PrimitiveVariable>(var_name, type, parse_expression());
            }
            return make_shared<PrimitiveVariable>(var_name, type);
        case TOKEN_ARRAY:
            return std::make_shared<ArrayVariable>(var_name, parse_array_type());
        default:
            throw std::runtime_error("type of variable is expected !");
    }
}

std::shared_ptr<If> Parser::parse_if_statement()
{
    auto result = std::make_shared<If>();

    if (currentTok().m_id != TOKEN_IF)
    {
        throw std::runtime_error("'if' is expected as the begin of if-statement!");
    }

    advanceTok();
    result->m_condition = parse_expression();

    if (currentTok().m_id != TOKEN_THEN)
    {
        throw std::runtime_error("'then' is expected as the begin of the true-block");
    }

    advanceTok();
    result->m_then = parse_body();

    if (currentTok().m_id != TOKEN_ELSE)
    {
        if (currentTok().m_id != TOKEN_END)
        {
            throw std::runtime_error("'else' or 'end' is expected as the begin of the false-block");
        }
        else
        {
            advanceTok();
            result->m_else = nullptr;
            return result;
        }
    }

    advanceTok();
    result->m_else = parse_body();

    consumeNewlines();

    if (currentTok().m_id != TOKEN_END)
    {
        throw std::runtime_error("'end' is expected as the end of the if-statement");
    }
    advanceTok();

    return result;
}

std::shared_ptr<For> Parser::parse_for_statement()
{
    auto result = std::make_shared<For>();

    if (currentTok().m_id != TOKEN_FOR)
    {
        throw std::runtime_error("'for' is expected as the begin of for-loop!");
    }

    advanceTok();
    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("identifier is expected at the for-loop!");
    }
    result->m_identifier
        = std::make_shared<PrimitiveVariable>(currentTok().m_value, std::make_shared<PrimitiveType>("integer"));

    advanceTok();
    result->m_range = parse_range();

    if (currentTok().m_id != TOKEN_LOOP)
    {
        throw std::runtime_error("'loop' is expected at the for-loop!");
    }
    advanceTok();

    result->m_body = parse_body();

    if (currentTok().m_id != TOKEN_END)
    {
        throw std::runtime_error("'end' is expected as the end of the for-loop");
    }
    advanceTok();

    return result;
}

std::shared_ptr<Range> Parser::parse_range()
{
    auto result = std::make_shared<Range>();

    if (currentTok().m_id != TOKEN_IN)
    {
        throw std::runtime_error("'in' is expected before the range!");
    }
    advanceTok();

    if (currentTok().m_id == TOKEN_REVERSE)
    {
        result->m_reverse = true;
        advanceTok();
    }

    result->m_begin = parse_expression();

    if (currentTok().m_id != TOKEN_DOTDOT)
    {
        throw std::runtime_error("'in' is expected before the range!");
    }
    advanceTok();

    result->m_end = parse_expression();

    return result;
}

std::shared_ptr<While> Parser::parse_while_statement()
{
    auto result = std::make_shared<While>();

    if (currentTok().m_id != TOKEN_WHILE)
    {
        throw std::runtime_error("'while' is expected before the while-loop!");
    }
    advanceTok();

    result->m_condition = parse_expression();

    if (currentTok().m_id != TOKEN_LOOP)
    {
        throw std::runtime_error("'loop' is expected at the while-loop!");
    }
    advanceTok();

    result->m_body = parse_body();

    if (currentTok().m_id != TOKEN_END)
    {
        throw std::runtime_error("'end' is expected as the end of the while-loop");
    }
    advanceTok();

    return result;
}

std::shared_ptr<Type> Parser::parse_type_decl()
{
    if (currentTok().m_id != TOKEN_TYPE)
    {
        throw std::runtime_error("'type' is expected to declare a type !");
    }

    advanceTok();
    if (currentTok().m_id != TOKEN_IDENTIFIER)
    {
        throw std::runtime_error("identifier expected");
    }
    auto name_of_the_type = currentTok().m_value;

    advanceTok();
    if (currentTok().m_id != TOKEN_IS)
    {
        throw std::runtime_error("'is' expected");
    }

    advanceTok();
    switch (currentTok().m_id)
    {
        case TOKEN_RECORD:
            return parse_record_decl(name_of_the_type);
        case TOKEN_ARRAY: {
            auto array_type = parse_array_type();
            return std::make_shared<TypeAliasing>(array_type, name_of_the_type);
        }
        case TOKEN_BOOLEAN:
        case TOKEN_INTEGER:
        case TOKEN_REAL:
        case TOKEN_IDENTIFIER: {
            const auto& value = currentTok().m_value;
            advanceTok();
            return std::make_shared<TypeAliasing>(std::make_shared<PrimitiveType>(value), name_of_the_type);
        }
        default:
            throw std::runtime_error("Specify the type being declared or aliased!");
    }
}

std::shared_ptr<Program> Parser::parse()
{
    auto result = std::make_shared<Program>();
    while (true)
    {
        auto token = currentTok();

        if (token.m_id == TOKEN_EOF)
        {
            break;
        }

        switch (token.m_id)
        {
            case TOKEN_ROUTINE:
                result->m_declarations.push_back(parse_routine_decl());
                break;
            case TOKEN_VAR:
                result->m_declarations.push_back(parse_variable_decl());
                break;
            case TOKEN_TYPE:
                result->m_declarations.push_back(parse_type_decl());
                break;
            case TOKEN_NEWLINE:
            case TOKEN_SEMICOLON:
                advanceTok();
                break;
            default:
                throw std::runtime_error("Failed to parse program in the space outside of routines !");
        }
    }
    return result;
}

} // namespace parsing
