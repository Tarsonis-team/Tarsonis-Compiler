#pragma once

#include "AST-node.hpp"
#include "declaration.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace parsing {

class Body : public ASTNode
{
public:
    explicit Body() : ASTNode(GrammarUnit::BODY)
    {
    }

    void print() override
    {
        std::cout << "Body:\n";
        for (auto& item : m_items)
        {
            item->print();
        }
        std::cout << "End of Body\n";
    }

    void checkUndecalredWithCopy(std::unordered_map<std::string, std::shared_ptr<Declaration>> table) {
        for (auto& stmt : m_items) {
            stmt->checkUndeclared(table);
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        std::unordered_map<std::string, int> variables = outer_table;
        std::unordered_set<std::string> shadows;

        for (auto& stmt : m_items) {
            // only var decls introduce new variable symbols
            if (stmt->isVariableDecl()) {
                auto& var = dynamic_cast<Variable&>(*stmt);
                if (outer_table.contains(var.m_name)) {
                    shadows.emplace(var.m_name);
                }
                if (var.m_value.get()) {
                    var.m_value->removeUnused(variables);
                }

                variables.emplace(var.m_name, 0);
                continue;
            }
            // here we calculate the usages of vars
            stmt->removeUnused(variables);
        }

        // remove stuff that is not used
        std::erase_if(m_items, [&variables](auto stmt){
            if (stmt->isVariableDecl()) {
                auto& var = dynamic_cast<Variable&>(*stmt);
                return variables.at(var.m_name) == 0;
            }
            return false;
        });

        // and then update counts in outer table.
        for (const auto& [variable, usages] : variables) {
            if (shadows.contains(variable)) {
                continue;
            }
            if (outer_table.contains(variable)) {
                outer_table[variable] = usages;
            }
        }

    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        // we make copy, because symbols declared in a particular body cant be seen 
        // outside
        checkUndecalredWithCopy(table);
    }

    std::vector<std::shared_ptr<ASTNode>> m_items;
    std::shared_ptr<Expression> m_return;
};

}  // namespace parsing
