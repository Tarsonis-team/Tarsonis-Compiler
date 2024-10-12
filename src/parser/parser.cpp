#include <memory>
#include <stdexcept>
#include <vector>
#include "parser.hpp"
// #include "lexer.hpp"
#include "token.hpp"
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
            declaration = std::make_shared<PrimitiveVariable>(var_name, currentTok().m_value);
            break;
        case TOKEN_ARRAY:
            declaration = std::make_shared<ArrayVariable>();
    }

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
