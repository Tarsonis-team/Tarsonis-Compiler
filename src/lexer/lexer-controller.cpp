#include "lexer-controller.hpp"
#include <string>
#include <vector>

namespace lexical
{

LexerController::LexerController(const std::string& source_file_path) : m_source_file(source_file_path, std::ios::in)
{
}

std::string LexerController::get_next_stripped_sequence()
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

std::vector<Token> LexerController::parse()
{
    std::vector<Token> result;
    while (!m_source_file.eof())
    {
        auto stripped = get_next_stripped_sequence();

        auto parsed = m_lexer.ParseStrippedSequence(stripped);

        for (auto& tok : parsed)
        {
            result.push_back(std::move(tok));
        }
    }
    return result;
}

} // namespace lexical
