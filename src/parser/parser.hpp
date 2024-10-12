#pragma once

#include <memory>
#include <vector>

#include "token.hpp"
#include "AST-node.hpp"
#include "declaration.hpp"
#include "statement.hpp"

namespace parsing {
class Parser
{
public:
    explicit Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {};
private:
    Program parse();

    Token currentTok();
    Token peekNextToken();
    void advanceTok();

    std::shared_ptr<Variable> parse_modifiable_primary();
    std::shared_ptr<Routine> parse_routine_decl();
    std::shared_ptr<Variable> parse_variable_decl();
    std::shared_ptr<Type> parse_type_decl();
    std::shared_ptr<RoutineParameter> parse_routine_parameter();
    std::shared_ptr<RecordType> parse_record_decl(std::string name);
    std::shared_ptr<ArrayType> parse_array_type();
    std::shared_ptr<ArrayVariable> parse_array_variable();
    std::shared_ptr<Expression> parse_expression();
    std::shared_ptr<Statement> parse_statement();
    std::shared_ptr<If> parse_if_statement();
    std::shared_ptr<For> parse_for_statement();
    std::shared_ptr<While> parse_while_statement();
    std::shared_ptr<Range> parse_range();
    std::shared_ptr<RoutineCall> parse_routine_call();
    std::shared_ptr<Assignment> parse_assignment();

    std::shared_ptr<Body> parse_body();

    size_t m_cur_tok;
    const std::vector<Token>& m_tokens;
};

}  // namespace parsing
