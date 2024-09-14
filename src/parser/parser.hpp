#pragma once

#include <fstream>
#include <vector>

#include "../lexer/token.hpp"
#include "tree.hpp"

namespace parse {
class Parser
{
public:
    explicit Parser(std::vector<Token>& tokens);
    Tree create_tree();
private:
    std::vector<Token> tokens;
};
}