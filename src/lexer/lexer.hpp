#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "token.hpp"

namespace lexical
{

class Lexer
{
public:
    explicit Lexer(const std::string& file_name);
    std::vector<Token> parse();

private:
    std::string get_next_stripped_sequence();
    std::ifstream m_source_file;
    std::vector<int> m_tokens;
};

} // namespace lexical
