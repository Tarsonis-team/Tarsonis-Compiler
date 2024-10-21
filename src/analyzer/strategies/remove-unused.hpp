#pragma once

#include "parser/AST-node.hpp"
#include <memory>

struct RemoveUnusedDeclarations
{
    explicit RemoveUnusedDeclarations(std::shared_ptr<parsing::Program> program) : m_ast(program) {}


    void apply()
    {
    }

    std::shared_ptr<parsing::Program> m_ast;
};
