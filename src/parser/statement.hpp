#pragma once

#include "AST-node.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include <iostream>
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
