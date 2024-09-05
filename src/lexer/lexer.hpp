#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "token.hpp"

class Lexer
{
public:
    std::fstream m_source_file;
    std::vector<int> m_tokens;

    explicit Lexer(const std::string& file_name);

    std::pair<std::string, char> get_word();
    std::vector<Token> parse();
};
