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

std::string Lexer::get_next_stripped_sequence()
{
    std::string stripped;
    while(m_source_file.peek() != std::char_traits<char>::eof()) {
        char ch = m_source_file.get();

        if (ch == ' ') {
            if (stripped.empty()) {
                continue;
            }
            return stripped;
        }

        stripped += ch;
    }

    return stripped;
}

std::vector<Token> Lexer::break_stripped_into_tokens(const std::string& stripped)
{

}

std::vector<Token> Lexer::parse()
{
    std::vector<Token> res;

    std::string tok;
    while (!m_source_file.eof()) {
        auto stripped = get_next_stripped_sequence();

        // there are cases like 'a:=4;' where there is no 
        // whitespace between the tokens, so we need additional
        // processing of the retreived stripped value.
        auto tokens = break_stripped_into_tokens(stripped);

    }
    
    return res;
}
