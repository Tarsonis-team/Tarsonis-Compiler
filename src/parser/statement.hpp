#pragma once

#include "AST-node.hpp"
#include "body.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include "routine.hpp"
#include <memory>
#include <vector>

namespace parsing
{

class If : public Statement
{
public:
    explicit If() : Statement(GrammarUnit::IF)
    {
    }

    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void checkReturnCoincides(
        std::shared_ptr<Type> type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override
    {
        m_then->checkReturnCoincides(type, table);
        if (m_else.get())
        {
            m_else->checkReturnCoincides(type, table);
        }
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        m_then->removeUnused(outer_table);
        if (m_else.get())
        {
            m_else->removeUnused(outer_table);
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

    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void checkReturnCoincides(
        std::shared_ptr<Type> type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override
    {
        m_body->checkReturnCoincides(type, table);
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        m_body->removeUnused(outer_table);
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

    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void checkReturnCoincides(
        std::shared_ptr<Type> type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override
    {
        m_body->checkReturnCoincides(type, table);
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        m_condition->removeUnused(outer_table);
        m_body->removeUnused(outer_table);
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

    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    std::string m_routine_name;
    std::vector<std::shared_ptr<Expression>> m_parameters;

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        for (const auto& param : m_parameters)
        {
            param->removeUnused(outer_table);
        }
    }
};

class RoutineCallResult : public Expression
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

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        m_routine_call->removeUnused(outer_table);
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& var_table,
    std::unordered_map<std::string, std::shared_ptr<Declaration>>& type_table) override
    {
        auto routine = std::static_pointer_cast<Routine>(var_table.at(m_routine_call->m_routine_name));
        return std::static_pointer_cast<Type>(type_table.at(routine->return_type));
    }

    explicit RoutineCallResult() : Expression()
    {
        this->m_grammar = GrammarUnit::ROUTINE_CALL;
    }

    std::shared_ptr<RoutineCall> m_routine_call;
};

class Assignment : public Statement
{
public:
    explicit Assignment() : Statement(GrammarUnit::ASSIGNMENT)
    {
    }

    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        m_modifiable->removeUnused(outer_table);
        m_expression->removeUnused(outer_table);
    }

    std::shared_ptr<Modifiable> m_modifiable;
    std::shared_ptr<Expression> m_expression;
};

} // namespace parsing
