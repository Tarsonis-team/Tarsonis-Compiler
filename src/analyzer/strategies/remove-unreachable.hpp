#pragma once

#include "parser/AST-node.hpp"
#include <memory>

struct RemoveUnreachableCode
{
    explicit RemoveUnreachableCode(std::shared_ptr<parsing::Program> program) : m_ast(program) {}

    void apply()
    {
        m_ast->removeUnreachable();
    }

    std::shared_ptr<parsing::Program> m_ast;
};
