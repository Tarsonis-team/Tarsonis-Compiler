#pragma once

#include "parser/declaration.hpp"
#include "parser/statement.hpp"
#include "parser/visitor/abstract-visitor.hpp"
#include "parser/AST-node.hpp"
#include "parser/std-function.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct RemoveUnusedDeclarations : public parsing::IVisitor
{
    explicit RemoveUnusedDeclarations(std::shared_ptr<parsing::Program> program) : m_ast(program)
    {
    }

    void visit(parsing::ASTNode& node) override
    {
        node.accept(*this);
    }

    void visit(parsing::Program& node) override
    {
        for (auto& entity : node.m_declarations)
        {
            entity->accept(*this);
        }
    }

    void visit(parsing::Declaration& node) override
    {
    }

    void visit(parsing::Variable& node) override {

    }

    void visit(parsing::Type& node) override
    {
    }

    void visit(parsing::TypeAliasing& node) override
    {
    }

    void visit(parsing::ArrayType& node) override
    {
    }

    void visit(parsing::ArrayVariable& node) override
    {
    }

    void visit(parsing::PrimitiveVariable& node) override
    {
        if (node.m_value) {
            node.m_value->accept(*this);
        }
    }

    void visit(parsing::Body& node) override
    {
        std::unordered_map<std::string, int> outer_scope = this->table;
        std::unordered_set<std::string> shadows;

        for (auto& stmt : node.m_items)
        {
            // only var decls introduce new variable symbols
            if (stmt->isVariableDecl())
            {
                auto& var = dynamic_cast<parsing::Variable&>(*stmt);
                if (outer_scope.contains(var.m_name)){
                    shadows.insert(var.m_name);
                }
                if (var.m_value.get())
                {
                    var.m_value->accept(*this);
                }

                this->table.emplace(var.m_name, 0);
                continue;
            }

            // here we calculate the usages of vars
            stmt->accept(*this);
        }

        // remove stuff that is not used
        std::erase_if(
            node.m_items,
            [this](auto stmt)
            {
                if (stmt->isVariableDecl())
                {
                    auto& var = dynamic_cast<parsing::Variable&>(*stmt);
                    return this->table.at(var.m_name) == 0;
                }
                return false;
            });

        // and then update counts in outer table.
        for (const auto& [variable, usages] : this->table)
        {
            // if this variable is from outside the scope, and is not shadowing
            if (outer_scope.contains(variable) && !shadows.contains(variable))
            {
                outer_scope[variable] = usages;
            }
        }
        this->table = std::move(outer_scope);
    }

    void visit(parsing::Routine& node) override
    {
        node.m_body->accept(*this);
    }

    void visit(parsing::RoutineCall& node) override
    {
        for (const auto& param : node.m_parameters)
        {
            param->accept(*this);
        }
    }

    void visit(parsing::StdFunction& node) override 
    {
        for (const auto& param : node.m_parameters)
        {
            param->accept(*this);
        }
    }

    void visit(parsing::RoutineCallResult& node) override
    {
        node.m_routine_call->accept(*this);
    }

    void visit(parsing::RoutineParameter& node) override
    {
    }

    void visit(parsing::Statement& node) override
    {
    }

    void visit(parsing::Expression& node) override
    {
    }

    void visit(parsing::True&) override
    {
    }

    void visit(parsing::False&) override
    {
    }

    void visit(parsing::Math& node) override
    {
        node.m_left->accept(*this);
        node.m_right->accept(*this);
    }

    void visit(parsing::Real& node) override
    {
    }

    void visit(parsing::Boolean& node) override
    {
    }

    void visit(parsing::Integer& node) override
    {
    }

    void visit(parsing::Modifiable& node) override
    {
        this->table[node.m_head_name] += 1;
    }

    void visit(parsing::ArrayAccess& node) override
    {
    }

    void visit(parsing::RecordAccess& node) override
    {
    }

    void visit(parsing::ReturnStatement& node) override
    {
        node.m_expr->accept(*this);
    }

    void visit(parsing::If& node) override
    {
        node.m_then->accept(*this);
        if (node.m_else.get())
        {
            node.m_else->accept(*this);
        }
    }

    void visit(parsing::Range& node) override
    {
    }

    void visit(parsing::For& node) override
    {
        node.m_body->accept(*this);
    }

    void visit(parsing::While& node) override
    {
        node.m_condition->accept(*this);
        node.m_body->accept(*this);
    }

    void visit(parsing::Assignment& node) override
    {
        node.m_modifiable->accept(*this);
        node.m_expression->accept(*this);
    }

    void visit(parsing::RecordType& node) override
    {
    }

    void apply()
    {
        m_ast->accept(*this);
    }

    std::shared_ptr<parsing::Program> m_ast;
    std::unordered_map<std::string, int> table;
};