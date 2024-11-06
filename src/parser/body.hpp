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

    explicit Body() : ASTNode(GrammarUnit::BODY)
    {
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        std::unordered_map<std::string, int> variables = outer_table;
        std::unordered_set<std::string> shadows;

        for (auto& stmt : m_items)
        {
            // only var decls introduce new variable symbols
            if (stmt->isVariableDecl())
            {
                auto& var = dynamic_cast<Variable&>(*stmt);
                if (outer_table.contains(var.m_name))
                {
                    shadows.emplace(var.m_name);
                }
                if (var.m_value.get())
                {
                    var.m_value->removeUnused(variables);
                }

                variables.emplace(var.m_name, 0);
                continue;
            }
            // here we calculate the usages of vars
            stmt->removeUnused(variables);
        }

        // remove stuff that is not used
        std::erase_if(
            m_items,
            [&variables](auto stmt)
            {
                if (stmt->isVariableDecl())
                {
                    auto& var = dynamic_cast<Variable&>(*stmt);
                    return variables.at(var.m_name) == 0;
                }
                return false;
            });

        // and then update counts in outer table.
        for (const auto& [variable, usages] : variables)
        {
            if (shadows.contains(variable))
            {
                continue;
            }
            if (outer_table.contains(variable))
            {
                outer_table[variable] = usages;
            }
        }
    }

    std::vector<std::shared_ptr<ASTNode>> m_items;
};

} // namespace parsing
