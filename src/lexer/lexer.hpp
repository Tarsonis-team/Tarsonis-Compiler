#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "token.hpp"

class Lexer
{
public:
    explicit Lexer(const std::string& file_name);
    std::vector<Token> parse();

private:
    std::string get_next_stripped_sequence();
    std::vector<Token> break_stripped_into_tokens(const std::string& stripped);
    std::ifstream m_source_file;
    std::vector<int> m_tokens;
};
