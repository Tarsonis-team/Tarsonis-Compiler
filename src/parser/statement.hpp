#pragma once

#include "AST-node.hpp"
#include "body.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include "routine.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace parsing
{

class If : public Statement
{
public:
    explicit If() : Statement(GrammarUnit::IF)
    {
    }

    void checkReturnCoincides(std::shared_ptr<Type> type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_then->checkReturnCoincides(type, table);
        if (m_else.get()) {
            m_else->checkReturnCoincides(type, table);
        }
    }
    
    void removeUnreachable() override {
        m_then->removeUnreachable();
        if (m_else.get()) {
            m_else->removeUnreachable();
        }
    };

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_then->checkUndeclared(table);
        if (m_else.get()) {
            m_else->checkUndeclared(table);
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        m_then->removeUnused(outer_table);
        if (m_else.get()) {
            m_else->removeUnused(outer_table);
        }
    }

    void print() override
    {
        std::cout << "If statement: condition:\n";
        std::cout << " ";

        m_condition->print();

        std::cout << "\nthen:\n";
        m_then->print();

        if (m_else.get())
        {
            std::cout << "\nelse:\n";
            m_else->print();
        }
        else
        {
            std::cout << "\nno else-part\n";
        }
    }

    std::shared_ptr<Expression> m_condition;
    std::shared_ptr<Body> m_then;
    std::shared_ptr<Body> m_else;
};

class For : public Statement
{
public:
    explicit For() : Statement(GrammarUnit::FOR)
    {
    }

    void checkReturnCoincides(std::shared_ptr<Type> type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_body->checkReturnCoincides(type, table);
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_body->checkUndeclared(table);
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        m_body->removeUnused(outer_table);
    }

    void removeUnreachable() override {
        m_body->removeUnreachable();
    };

    void print() override
    {
        std::cout << "For ";
        m_identifier->print();

        std::cout << " ";
        m_range->print();

        cout << "\n";

        m_body->print();
        std::cout << "End of For statement\n";
    }

    std::shared_ptr<Range> m_range;
    std::shared_ptr<Body> m_body;
    std::shared_ptr<Variable> m_identifier;
};

class While : public Statement
{
public:
    explicit While() : Statement(GrammarUnit::WHILE)
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_condition->checkUndeclared(table);
        m_body->checkUndeclared(table);
    }

    void checkReturnCoincides(std::shared_ptr<Type> type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_body->checkReturnCoincides(type, table);
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        m_condition->removeUnused(outer_table);
        m_body->removeUnused(outer_table);
    }

    void removeUnreachable() override {
        m_body->removeUnreachable();
    };

    void print() override
    {
        std::cout << "While statement cond:\n";
        m_condition->print();
        std::cout << "\n";
        m_body->print();
        std::cout << "End of While statement\n";
    }

    std::shared_ptr<Body> m_body;
    std::shared_ptr<Expression> m_condition;
};

class RoutineCall : public Statement
{
public:
    explicit RoutineCall(std::string name) : Statement(GrammarUnit::CALL), m_routine_name(std::move(name))
    {
    }

    std::string m_routine_name;
    std::vector<std::shared_ptr<Expression>> m_parameters;

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        if (!table.contains(m_routine_name)) {
            throw std::runtime_error("undeclared function is called: " + m_routine_name);
        }
        for (auto& param : m_parameters) {
            param->checkUndeclared(table);
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        for (const auto& param: m_parameters) {
            param->removeUnused(outer_table);
        }
    }

    void print() override
    {
        cout << "Calling routine: " << m_routine_name << " with params: ( ";

        for (auto& par : m_parameters)
        {
            par->print();
            cout << ",";
        }
        cout << ") ";
    }
};

class RoutineCallResult : public Expression
{
public:
    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_routine_call->checkUndeclared(table);

        // okay, after that check, we know this routine exists in the table.
        auto actual_routine = std::static_pointer_cast<Routine>(table.at(m_routine_call->m_routine_name));
        auto actual_parameters = actual_routine->m_params;

        if (actual_parameters.size() != m_routine_call->m_parameters.size()) {
            throw std::runtime_error("function signature mismatch: " + actual_routine->m_name);
        }

        for (size_t idx = 0; idx < actual_parameters.size(); ++idx) {
            auto type = std::static_pointer_cast<Type>(table.at(actual_parameters[idx]->m_type));
            if (*m_routine_call->m_parameters[idx]->deduceType(table) != *type) {
                throw std::runtime_error("function signature mismatch: " + actual_routine->m_name);
            }
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        m_routine_call->removeUnused(outer_table);
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        auto routine = std::static_pointer_cast<Routine>(table.at(m_routine_call->m_routine_name));
        return std::static_pointer_cast<Type>(table.at(routine->return_type));
    }

    explicit RoutineCallResult() : Expression()
    {
        this->m_grammar = GrammarUnit::ROUTINE_CALL;
    }

    std::shared_ptr<RoutineCall> m_routine_call;

    void print() override
    {
        m_routine_call->print();
    }
};

class Assignment : public Statement
{
public:
    explicit Assignment() : Statement(GrammarUnit::ASSIGNMENT)
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_modifiable->checkUndeclared(table);
        m_expression->checkUndeclared(table);

        auto modif_type = m_modifiable->deduceType(table);
        auto exp_type = m_expression->deduceType(table);

        if (*modif_type != *exp_type) {
            throw std::runtime_error("The assigned type does not match declared: " + modif_type->m_name + " is not " + exp_type->m_name);
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        m_modifiable->removeUnused(outer_table);
        m_expression->removeUnused(outer_table);
    }

    void print() override
    {
        std::cout << "Assignment ";
        m_modifiable->print();
        std::cout << " to\n";
        m_expression->print();
        std::cout << "\n";
    }

    std::shared_ptr<Modifiable> m_modifiable;
    std::shared_ptr<Expression> m_expression;
};

} // namespace parsing
