#pragma once

#include "parser/AST-node.hpp"
#include <memory>

struct RemoveUnusedDeclarations
{
    explicit RemoveUnusedDeclarations(std::shared_ptr<parsing::Program> program) : m_ast(program)
    {
    }

    void apply()
    {
        std::unordered_map<std::string, int> table;
        m_ast->removeUnused(table);
    }

    std::shared_ptr<parsing::Program> m_ast;
};
