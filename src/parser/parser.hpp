#pragma once

#include <fstream>
#include <vector>

#include "../lexer/token.hpp"
#include "tree/tree.hpp"
#include "utils/declarations/abstract_declaration.cpp"

namespace parse {
class Parser
{
public:
    explicit Parser(std::vector<Token>& tokens);
private:
    Tree<Declaration> get_decl_tree();
    std::vector<Token> tokens;
};
}