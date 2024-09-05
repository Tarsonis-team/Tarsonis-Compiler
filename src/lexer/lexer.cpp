#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <utility>

#include "token.hpp"
#include "lexer.hpp"

using namespace std;

Lexer::Lexer(const std::string& file_name)
{
    m_source_file.open(file_name, std::ios::in);
}

pair<string, char> Lexer::get_word()
{
    char ch;
    pair<string, char> word;

    while (true) {
        m_source_file.get(ch);
        if (ch == '.') {
            // ...
        }
        // TODO: handle each punctuation syntax

        if (ch == ' ') {
            word.second = ' ';
            return word;
        }

        if (ch == '\n') {
            word.second = '\n';
            return word;
        }

        word.first += ch;
    }
}

std::vector<Token> Lexer::parse()
{
    std::vector<Token> res;

    pair<string, char> tok;
    while (m_source_file.eof()) {
        tok = get_word();

        int word_id = tokens.at(tok.first);
        switch (word_id)
            {
                case 1:
                    res.emplace_back(tok.first, word_id);
            }


    }
    
    return res;
}
