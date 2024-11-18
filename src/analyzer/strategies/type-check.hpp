#pragma once

#include "parser/AST-node.hpp"
#include "parser/declaration.hpp"
#include "parser/expression.hpp"
#include "parser/statement.hpp"
#include "parser/visitor/abstract-visitor.hpp"
#include "parser/return.hpp"
#include "parser/routine.hpp"
#include "parser/std-function.hpp"
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct TypeCheck : parsing::IVisitor
{
    explicit TypeCheck(std::shared_ptr<parsing::Program> program) : m_ast(program)
    {
    }

    std::vector<std::string> checkErrors()
    {
        // what this table is supposed to hold?
        // typename -> object of the type
        // varname  -> object of the type
        m_type_table.emplace("integer", std::make_shared<parsing::PrimitiveType>("integer"));
        m_type_table.emplace("real", std::make_shared<parsing::PrimitiveType>("real"));
        m_type_table.emplace("boolean", std::make_shared<parsing::PrimitiveType>("boolean"));
        m_type_table.emplace("array", std::make_shared<parsing::PrimitiveType>("array"));

        try
        {
            m_ast->accept(*this);
        }
        catch (const std::exception& err)
        {
            throw;
            return { err.what() };
        }
        std::cout << "TypeCheck reports no errors.\n";
        return {};
    }

    void visit(parsing::ASTNode& node) override
    {
        // this enables double-dispatching in our code.
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

    void visit(parsing::Type& node) override
    {

    }

    void visit(parsing::TypeAliasing& node) override
    {
        if (!m_type_table.contains(node.m_from->m_name))
        {
            throw std::runtime_error("unknown type: " + node.m_from->m_name);
        }
        if (node.m_from->m_name == "array") {
            m_type_table.emplace(node.m_to, node.m_from);
        }
        m_type_table.emplace(node.m_to, m_type_table.at(node.m_from->m_name));
    }

    void visit(parsing::ArrayType& node) override
    {
        if (!m_type_table.contains(node.m_type->m_name))
        {
            throw std::runtime_error("Unknown type: " + node.m_type->m_name);
        }
        if (!node.m_size->isConst()) {
            throw std::runtime_error("Arrays can be of constant size only");
        }
    }

    void visit(parsing::Variable& node) override {

    }

    void visit(parsing::ArrayVariable& node) override
    {
        node.m_type->accept(*this);
        m_var_table.emplace(node.m_name, node.m_type);
    }

    void visit(parsing::PrimitiveVariable& node) override
    {
        if (!m_type_table.contains(node.m_type->m_name))
        {
            throw std::runtime_error("Unknown type: " + node.m_type->m_name);
        }
        if (node.m_assigned.get())
        {
            node.m_assigned->accept(*this);
        }
        m_var_table.emplace(node.m_name, m_type_table.at(node.m_type->m_name));
    }

    void visit(parsing::Body& node) override
    {
        // we copy all the variables and types...
        // yes, inneficient, but easy and it works
        // TODO(do wihout copy): optimize it...
        auto vars = m_var_table;
        auto types = m_type_table;
        for (auto& stmt : node.m_items)
        {
            stmt->accept(*this);
        }

        // and then we restore all that, so no variables leak
        // into other contexts
        m_var_table = std::move(vars);
        m_type_table = std::move(types);
    }

    void visit(parsing::Routine& node) override
    {
        for (auto& param : node.m_params)
        {
            param->accept(*this);
            
            m_type_table.emplace(param->m_name, m_type_table.at(param->m_type));
            m_var_table.emplace(param->m_name, m_type_table.at(param->m_type));
        }

        if (!node.return_type.empty() && !m_type_table.contains(node.return_type))
        {
            throw std::runtime_error("function returns an unknown type: " + node.return_type);
        }

        m_var_table.insert({ node.m_name, std::make_shared<parsing::Routine>(node) });
        if (!node.return_type.empty()) {
            m_current_return_type = std::dynamic_pointer_cast<parsing::Type>(m_type_table.at(node.return_type));
        }
        node.m_body->accept(*this);

        for (auto& param : node.m_params)
        {
            m_var_table.erase(param->m_name);
        }
    }

    void visit(parsing::RoutineCall& node) override
    {
        if (!m_var_table.contains(node.m_routine_name))
        {
            throw std::runtime_error("undeclared function is called: " + node.m_routine_name);
        }
        for (auto& param : node.m_parameters)
        {
            param->accept(*this);
        }
    }

    void visit(parsing::StdFunction& node) override {
        if (!parsing::StdFunction::is_std_function(node.m_routine_name)) {
            throw std::runtime_error("unknown std function is called: " + node.m_routine_name);
        }
        for (auto& param : node.m_parameters) {
            param->accept(*this);
        }
    }

    void visit(parsing::RoutineCallResult& node) override
    {
        node.m_routine_call->accept(*this);

        // okay, after that check, we know this routine exists in the table.
        auto actual_routine = std::static_pointer_cast<parsing::Routine>(m_var_table.at(node.m_routine_call->m_routine_name));
        auto actual_parameters = actual_routine->m_params;

        if (actual_parameters.size() != node.m_routine_call->m_parameters.size())
        {
            throw std::runtime_error("function signature mismatch: " + actual_routine->m_name);
        }

        for (size_t idx = 0; idx < actual_parameters.size(); ++idx)
        {
            auto type = std::static_pointer_cast<parsing::Type>(m_type_table.at(actual_parameters[idx]->m_type));
            if (*node.m_routine_call->m_parameters[idx]->deduceType(m_var_table, m_type_table) != *type)
            {
                throw std::runtime_error("function signature mismatch: " + actual_routine->m_name);
            }
        }
    }

    void visit(parsing::RoutineParameter& node) override
    {
        if (!m_type_table.contains(node.m_type))
        {
            throw std::runtime_error("function takes a parameter of an unknown type: " + node.m_type);
        }
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
        if (!m_var_table.contains(node.m_head_name))
        {
            throw std::runtime_error("unknown identifier: " + node.m_head_name);
        }

        if (node.m_chain.empty())
        {
            return;
        }

        auto type = m_var_table.at(node.m_head_name);
        // dealing with records.... or an array access... or both (
        for (auto& access : node.m_chain)
        {
            access->check_has_field(type, m_type_table);
        }
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
        auto type = node.m_expr->deduceType(m_var_table, m_type_table);
        if (*type != *m_current_return_type) {
            throw std::runtime_error("return type does not match: " + m_current_return_type->m_name);
        }
    }

    void visit(parsing::If& node) override
    {
        node.m_condition->accept(*this);
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
        node.m_identifier->accept(*this);
        node.m_body->accept(*this);
        m_var_table.erase(node.m_identifier->m_name);
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

        auto modif_type = node.m_modifiable->deduceType(m_var_table, m_type_table); // needs both (
        auto exp_type = node.m_expression->deduceType(m_var_table, m_type_table);

        if (*modif_type != *exp_type)
        {
            throw std::runtime_error(
                "The assigned type does not match declared: " + modif_type->m_name + " is not " + exp_type->m_name);
        }
    }

    void visit(parsing::RecordType& node) override
    {
        for (auto& field : node.m_fields)
        {
            auto* var = dynamic_cast<parsing::Variable*>(field.get());
            var->m_type->accept(*this);
        }

        m_type_table.emplace(node.m_name, std::make_shared<parsing::RecordType>(node));
    }

    std::shared_ptr<parsing::Type> m_current_return_type;
    std::shared_ptr<parsing::Program> m_ast;
    std::unordered_map<std::string, std::shared_ptr<parsing::Declaration>> m_type_table;
    std::unordered_map<std::string, std::shared_ptr<parsing::Declaration>> m_var_table;
};
