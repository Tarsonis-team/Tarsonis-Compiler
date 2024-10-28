#pragma once

#include "AST-node.hpp"
#include "declaration.hpp"
#include "grammar-units.hpp"
#include <memory>
#include <stdexcept>

namespace parsing {

class ReturnStatement : public Statement {
public:
    explicit ReturnStatement(std::shared_ptr<Expression> expr) : Statement(GrammarUnit::RETURN), m_expr(expr) {}

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_expr->checkUndeclared(table);
    }

    void checkReturnCoincides(std::shared_ptr<Type> awaited, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        auto type = m_expr->deduceType(table);
        if (*awaited != *type) {
            throw std::runtime_error("function returns different type from declared");
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override {
        m_expr->removeUnused(table);
    }

    std::shared_ptr<Expression> m_expr;
};

} // namespace parsing
