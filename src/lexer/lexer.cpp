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

void Lexer::first_split() {

}

void Lexer::second_split() {

}

string Lexer::get_word()
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
//            return word;
        }

        if (ch == '\n') {
            word.second = '\n';
//            return word;
        }

        word.first += ch;
    }
}

// Separate function for checking punct after words
void punct_handler(char punct, vector<Token>& res) {
    if (punct == ' ') {
        return;
    }

    string punct_string;
    punct_string.push_back(punct);

    res.emplace_back(punct_string, tokens.at(punct_string));
}

std::vector<Token> Lexer::parse()
{
    std::vector<Token> res;

    pair<string, char> tok;
    while (m_source_file.eof()) {
//        tok = get_word();

        if (!tokens.count(tok.first)) {
            res.emplace_back(tok.first, 0);
            punct_handler(tok.second, res);
        }

        int word_id = tokens.at(tok.first);
        res.emplace_back(tok.first, word_id);

        punct_handler(tok.second, res);
    }
    
    return res;
}
