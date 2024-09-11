#pragma once

#include <string>
#include <vector>

#include "token.hpp"

namespace lexical
{

class Lexer
{
public:
    explicit Lexer() = default;
    std::vector<Token> ParseStrippedSequence(const std::string& stripped);
};

} // namespace lexical
