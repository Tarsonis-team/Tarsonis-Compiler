#pragma once

#include <memory>
#include <vector>

#include "token.hpp"
#include "AST-node.hpp"
#include "declaration.hpp"

namespace parsing {
class Parser
{
public:
    explicit Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {};
private:
    Program parse();

    Token currentTok();
    void advanceTok();

    std::shared_ptr<Routine> parse_routine_decl();
    std::shared_ptr<Variable> parse_variable_decl();
    std::shared_ptr<Type> parse_type_decl();
    std::shared_ptr<RoutineParameter> parse_routine_parameter();
    std::shared_ptr<Record> parse_record_decl();
    std::shared_ptr<Array> parse_array_type();
    std::shared_ptr<ArrayVariable> parse_array_variable();
    std::shared_ptr<Expression> parse_expression();


    size_t m_cur_tok;
    const std::vector<Token>& m_tokens;
};

}  // namespace parsing
