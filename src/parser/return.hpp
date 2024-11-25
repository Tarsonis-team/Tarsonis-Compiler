#pragma once

#include "AST-node.hpp"
#include "declaration.hpp"
#include "grammar-units.hpp"
#include <memory>
#include <stdexcept>

namespace parsing
{

class ReturnStatement : public Statement
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

    explicit ReturnStatement(std::shared_ptr<Expression> expr) : Statement(GrammarUnit::RETURN), m_expr(expr)
    {
    }

    void checkReturnCoincides(
        std::shared_ptr<Type> awaited, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override
    {
        // auto type = m_expr->deduceType(table);
        // if (*awaited != *type)
        // {
        //     throw std::runtime_error("function returns different type from declared");
        // }
    }

    std::shared_ptr<Expression> m_expr;
};

} // namespace parsing
