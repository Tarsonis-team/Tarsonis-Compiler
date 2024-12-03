#pragma once

#include "AST-node.hpp"
#include "declaration.hpp"
#include "grammar-units.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace parsing
{

class Body : public ASTNode
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit Body() : ASTNode(GrammarUnit::BODY)
    {
    }

    std::vector<std::shared_ptr<ASTNode>> m_items;
};

} // namespace parsing
