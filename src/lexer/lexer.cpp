#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "sequence-breaker.hpp"
#include "token.hpp"

namespace
{

bool is_reserved_keyword(const std::string& sequence)
{
    return tokens.contains(sequence);
}

bool is_constant(const std::string& sequence)
{
    return std::all_of(sequence.begin(), sequence.end(), [](unsigned char c) { return std::isdigit(c); });
}

} // namespace

namespace lexical
{

Lexer::Lexer(const std::string& file_name) : m_source_file(file_name, std::ios::in)
{
}

std::string Lexer::get_next_stripped_sequence()
{
    std::string stripped;
    while (m_source_file.peek() != std::char_traits<char>::eof())
    {
        char ch = m_source_file.get();

        if (ch == ' ')
        {
            if (stripped.empty())
            {
                continue;
            }
            return stripped;
        }

        stripped += ch;
    }

    return stripped;
}

std::vector<Token> Lexer::parse()
{
    std::vector<Token> res;

    std::string tok;
    while (!m_source_file.eof())
    {
        auto stripped = get_next_stripped_sequence();

        if (is_reserved_keyword(stripped))
        {
            res.push_back(Token::asReservedKeyword(stripped));
            continue;
        }

        if (is_constant(stripped))
        {
            res.push_back(Token::asConstant(stripped));
            continue;
        }

        // there are cases like 'a:=4;' where there is no
        // whitespace between the tokens, so we need additional
        // processing of the retreived stripped value.

        // as for this call - we expect this:
        // a:=5; -> a, :=, 5, ;
        // (3-2/5+12 - 1) -> (, 3, -, 2, /, 5, +, 12, -, 1, )
        const auto broken = SequenceBreaker(stripped)
                                .breakBy(":=")
                                .breakBy(":")
                                .breakBy(";")
                                .breakBy(".")
                                .breakBy("*")
                                .breakBy("/")
                                .breakBy("+")
                                .breakBy("-")
                                .breakBy("%")
                                .breakBy("\n")
                                .breakBy("[")
                                .breakBy("]")
                                .breakBy("(")
                                .breakBy(")")
                                .breakBy("<")
                                .breakBy(">")
                                .breakBy(">=")
                                .breakBy("<=")
                                .breakBy("->")
                                .done();

        for (const auto& tok : broken)
        {
            if (is_reserved_keyword(tok))
                res.push_back(Token::asReservedKeyword(tok));
            else if (is_constant(tok))
                res.push_back(Token::asConstant(tok));
            else
                res.push_back(Token::asIdentifier(tok));
        }
    }

    return res;
}

} // namespace lexical
