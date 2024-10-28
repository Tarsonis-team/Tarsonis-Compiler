#pragma once

#include "declaration.hpp"
#include "body.hpp"
#include <memory>

namespace parsing {

class Routine : public Declaration
{
public:
    explicit Routine(std::string name) : Declaration(GrammarUnit::ROUTINE, std::move(name))
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        for (auto& param : m_params) {
            param->checkUndeclared(table);
            table.emplace(param->m_name, table.at(param->m_type));
        }

        if (!return_type.empty() && !table.contains(return_type)) {
            throw std::runtime_error("function returns an unknown type: " + return_type);
        }

        table.insert({m_name, std::static_pointer_cast<Declaration>(shared_from_this())});

        m_body->checkUndeclared(table);
        if (!return_type.empty()) {
            m_body->checkReturnCoincides(std::static_pointer_cast<Type>(table.at(return_type)), table);
        }
        
        for (auto& param : m_params) {
            table.erase(param->m_name);
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override {
        m_body->removeUnused(table);
    }

    void print() override
    {
        cout << "Routine declaration, name: " << m_name << " -> " << (return_type.empty() ? "void" : return_type)
             << '\n';
        m_body->print();
        cout << "\n";
        cout << "End of routine declaration\n";
    }

    std::shared_ptr<Body> m_body;
    std::vector<std::shared_ptr<RoutineParameter>> m_params;
    std::string return_type;
};

}  // namespace parsing
