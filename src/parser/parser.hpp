#pragma once

#include <vector>
#include "../lexer/lexer.hpp"
#include "utils/declarations/AST-node.hpp"

namespace parsing {
class Parser
{
public:
    explicit Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {};
private:
    Program parse();
    const std::vector<Token>& m_tokens;
};
}