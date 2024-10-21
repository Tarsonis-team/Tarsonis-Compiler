#pragma once

#include "parser/AST-node.hpp"
#include <memory>
#include <string>
#include <vector>

struct CheckUndeclaredSymbols
{
    explicit CheckUndeclaredSymbols(std::shared_ptr<parsing::Program> program) : m_ast(program) {}


    std::vector<std::string> checkErrors()
    {
        return {};
    }

    std::shared_ptr<parsing::Program> m_ast;
};
