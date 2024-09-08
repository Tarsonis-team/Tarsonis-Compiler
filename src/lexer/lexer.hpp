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

    std::vector<std::string> words;

    explicit Lexer(const std::string& file_name);

    std::string get_word();
    std::vector<Token> parse();

    void first_split();
    void second_split();
};
