#include <memory>
#include <stdexcept>
#include <vector>
#include "parser.hpp"
// #include "lexer.hpp"
#include "../lexer/token.hpp"
#include "AST-node.hpp"
#include "declaration.hpp"

namespace parsing {

Token Parser::currentTok() {
    return m_tokens[m_cur_tok];
}

void Parser::advanceTok() {
    ++m_cur_tok;
}

std::shared_ptr<RoutineParameter> Parser::parse_routine_parameter() {
    auto type = currentTok().m_id;
    if (type != TOKEN_IDENTIFIER && type != TOKEN_INTEGER && type != TOKEN_BOOLEAN && type != TOKEN_REAL) {
        throw std::runtime_error("Identifier expected or primitive type expected");
    }
    auto type_identifier = currentTok().m_value;
    advanceTok();

    if (currentTok().m_id != TOKEN_IDENTIFIER) {
        throw std::runtime_error("Identifier expected");
    }
    auto var_name = currentTok().m_value;
    advanceTok();

    return std::make_shared<RoutineParameter>(var_name, type_identifier);
}

std::shared_ptr<Routine> Parser::parse_routine_decl() {
    advanceTok();
    if (currentTok().m_id != TOKEN_IDENTIFIER) {
        throw std::runtime_error("Identifier was expected !");
    }
    Routine res(currentTok().m_value);
    
    advanceTok();
    if (currentTok().m_id != TOKEN_LPAREN) {
        throw std::runtime_error("'(' expected");
    }

    // карусель, карусель, это радость для нас ! парсим ебаные параметры
    // they can only be in form of [primitive_type | identifier_of_type] identifier 
    advanceTok();
    while (true) {
        if (currentTok().m_id != TOKEN_COMA) {
            break;
        }
        res.m_params.push_back(std::make_shared<RoutineParameter>(parse_routine_parameter()));
    }

    if (currentTok().m_id != TOKEN_RPAREN) {
        throw std::runtime_error("')' expected");
    }
    advanceTok();
    
    if (currentTok().m_id == TOKEN_ARROW) {
        auto type = currentTok().m_id;
        if (type != TOKEN_IDENTIFIER && type != TOKEN_INTEGER && type != TOKEN_BOOLEAN && type != TOKEN_REAL) {
            throw std::runtime_error("Identifier expected or primitive type expected");
        }
        res.return_type = currentTok().m_value;
    }
    advanceTok();

    if (currentTok().m_id != TOKEN_IS) {
        throw std::runtime_error("'is' expected");
    }





}

std::shared_ptr<Body> Parser::parse_body() {

}

std::shared_ptr<RecordType> Parser::parse_record_decl() {

}

std::shared_ptr<ArrayType> Parser::parse_array_type() {
    if (currentTok().m_id != TOKEN_ARRAY) {
        throw std::runtime_error("'array' token is expected!");
    }

    advanceTok();
    if (currentTok().m_id != TOKEN_LBRACKET) {
        throw std::runtime_error("'[' is expected !");
    }
    advanceTok();

    std::shared_ptr<Expression> number_of_elements = parse_expression();
    
    if (currentTok().m_id != TOKEN_RBRACKET) {
        throw std::runtime_error("']' is expected !");
    }

    advanceTok();
    switch (currentTok().m_id) {
        case TOKEN_IDENTIFIER:
            return std::make_shared<ArrayType>(CustomType(currentTok().m_value), number_of_elements);
        case TOKEN_BOOLEAN:
            return std::make_shared<ArrayType>(BoolType(), number_of_elements);
        case TOKEN_INTEGER:
            return std::make_shared<ArrayType>(IntType(), number_of_elements);
        case TOKEN_REAL:
            return std::make_shared<ArrayType>(RealType(), number_of_elements);
        default:
            throw std::runtime_error("Expected a type of the array !");
    }    
}


std::shared_ptr<Variable> Parser::parse_variable_decl() {
    if (currentTok().m_id != TOKEN_VAR) {
        throw std::runtime_error("var is expected to make a variable decl !");
    }
    advanceTok();

    if (currentTok().m_id != TOKEN_IDENTIFIER) {
        throw std::runtime_error("identifier to declare a var name is expected !");
    }
    auto var_name = currentTok().m_value;

    advanceTok();

    if (currentTok().m_id != TOKEN_COLON) {
        throw std::runtime_error("colon expected !");
    }
    advanceTok();
    
    std::shared_ptr<Variable> declaration;
    switch (currentTok().m_id) {
        case TOKEN_BOOLEAN:
        case TOKEN_INTEGER:
        case TOKEN_REAL:
            return std::make_shared<PrimitiveVariable>(var_name, currentTok().m_value);
        case TOKEN_ARRAY:
            return std::make_shared<ArrayVariable>(var_name, parse_array_type());
        case TOKEN_IDENTIFIER:
            return std::make_shared<RecordVariable>(var_name, Type(currentTok().m_value));
        default:
            throw std::runtime_error("type of variable is expected !");
    }
}



std::shared_ptr<If> Parser::parse_if_statement() {
    auto result = std::make_shared<If>();

    if (currentTok().m_id != TOKEN_IF) {
        throw std::runtime_error("'if' is expected as the begin of if-statement!");
    }

    advanceTok();
    result->m_condition = parse_expression();

    if (currentTok().m_id != TOKEN_THEN) {
        throw std::runtime_error("'then' is expected as the begin of the true-block");
    }

    advanceTok();
    result->m_then = parse_body();

    if (currentTok().m_id != TOKEN_ELSE) {
        if (currentTok().m_id != TOKEN_END) {
            throw std::runtime_error("'else' or 'end' is expected as the begin of the false-block");
        } else {
            result->m_else = nullptr;
            return result;
        }
    }

    advanceTok();
    result->m_else = parse_body();

    if (currentTok().m_id != TOKEN_END) {
        throw std::runtime_error("'end' is expected as the end of the if-statement");
    }

    return result;
}

std::shared_ptr<For> Parser::parse_for_statement() {
    auto result = std::make_shared<For>();

    if (currentTok().m_id != TOKEN_FOR) {
        throw std::runtime_error("'for' is expected as the begin of for-loop!");
    }

    advanceTok();
    if (currentTok().m_id != TOKEN_IDENTIFIER) {
        throw std::runtime_error("identifier is expected at the for-loop!");
    }
    result->m_identifier = std::make_shared<PrimitiveVariable>(currentTok().m_value, "integer");

    advanceTok();
    result->m_range = parse_range();

    if (currentTok().m_id != TOKEN_LOOP) {
        throw std::runtime_error("'loop' is expected at the for-loop!");
    }
    advanceTok();

    result->m_body = parse_body();

    if (currentTok().m_id != TOKEN_END) {
        throw std::runtime_error("'end' is expected as the end of the for-loop");
    }

    return result;
}

std::shared_ptr<Range> Parser::parse_range() {
    auto result = std::make_shared<Range>();

    if (currentTok().m_id != TOKEN_IN) {
        throw std::runtime_error("'in' is expected before the range!");
    }
    advanceTok();

    if (currentTok().m_id == TOKEN_REVERSE) {
        result->m_reverse = true;
        advanceTok();
    }

    result->m_begin = parse_expression();

    if (currentTok().m_id != TOKEN_DOTDOT) {
        throw std::runtime_error("'in' is expected before the range!");
    }
    advanceTok();

    result-> m_end = parse_expression();

    return result;
}

std::shared_ptr<While> Parser::parse_while_statement() {
    auto result = std::make_shared<While>();

    if (currentTok().m_id != TOKEN_WHILE) {
        throw std::runtime_error("'while' is expected before the while-loop!");
    }
    advanceTok();

    result->m_condition = parse_expression();

    if (currentTok().m_id != TOKEN_LOOP) {
        throw std::runtime_error("'loop' is expected at the while-loop!");
    }
    advanceTok();

    result->m_body = parse_body();

    if (currentTok().m_id != TOKEN_END) {
        throw std::runtime_error("'end' is expected as the end of the while-loop");
    }

    return result;
}

std::shared_ptr<Type> Parser::parse_type_decl() {
    if (currentTok().m_id != TOKEN_TYPE) {
        throw std::runtime_error("'type' is expected to declare a type !");
    }

    advanceTok();
    if (currentTok().m_id != TOKEN_IDENTIFIER) {
        throw std::runtime_error("identifier expected");
    }
    auto name_of_the_type = currentTok().m_value;

    advanceTok();
    if (currentTok().m_id != TOKEN_IS) {
        throw std::runtime_error("'is' expected");
    }

    advanceTok();
    switch (currentTok().m_id) {
        case TOKEN_RECORD:
            return parse_record_decl();
        case TOKEN_ARRAY:
            return parse_array_type();
        case TOKEN_BOOLEAN:
        case TOKEN_INTEGER:
        case TOKEN_REAL:
            return std::make_shared<TypeAliasing>(currentTok().m_value, name_of_the_type);
        default:
            throw std::runtime_error("Specify the type being declared or aliased!");
    }

}

Program Parser::parse() {
    Program result;
    int cur_token = 0;
    while (true) {
        auto token = currentTok();

        switch (token.m_id) {
            case TOKEN_ROUTINE:
                result.m_declarations.push_back(parse_routine_decl());
                break;
            case TOKEN_VAR:
                result.m_declarations.push_back(parse_variable_decl());
                break;
            case TOKEN_TYPE:
                result.m_declarations.push_back(parse_type_decl());
                break;
            default:
                advanceTok();
                break;
        }
    }
    
    // return {};
}




}  // namespace parsing
