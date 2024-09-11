#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "token.hpp"

namespace lexical
{


/**
 *   The following class is introduced to segregate 
 *   working with files from tokenizing them, this class
 *   is responsible for reading a file and feeding strings
 *   to the lexer. Previously, Lexer was both reading the 
 *   file and parsing it, which contradicts single-resp principle.
 *   It was also harder to unit-test the lexer.
 */
class LexerController
{
public:
    explicit LexerController(const std::string& source_file_path);
    std::vector<Token> parse();

private:
    std::string get_next_stripped_sequence();
    std::ifstream m_source_file;
    Lexer m_lexer;
};

} // namespace lexical