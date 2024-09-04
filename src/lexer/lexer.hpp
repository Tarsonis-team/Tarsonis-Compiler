#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "token.hpp"

class Lexer
{
public:
    std::ifstream m_source_file;
    std::vector<int> m_tokens;

    explicit Lexer(const std::string& file_name);
    
    std::string get_word();
    std::vector<Token> parse();
};
