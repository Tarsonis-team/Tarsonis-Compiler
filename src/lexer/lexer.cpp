#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "token.hpp"
#include "lexer.hpp"

Lexer::Lexer(const std::string& file_name)
{
    m_source_file.open(file_name, std::ios::in);
}

std::string Lexer::get_word()
{
    return "for";
}

std::vector<Token> Lexer::parse()
{
    std::vector<Token> res;

    std::string tok;
    while (m_source_file.eof()) {
        m_source_file >> tok;

        int word_id = tokens.at(tok);
        switch (word_id)
            {
                case 1:
                    res.emplace_back(tok, word_id);
            }
    }
    
    return res;
}
