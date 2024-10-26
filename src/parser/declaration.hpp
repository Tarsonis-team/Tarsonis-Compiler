#pragma once

#include "AST-node.hpp"
#include "grammar-units.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace parsing
{


class RoutineParameter : public Declaration
{
public:
    RoutineParameter(std::string name, std::string type)
        : Declaration(GrammarUnit::PARAMETER, std::move(name)), m_type(std::move(type))
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        if (!table.contains(m_type)) {
            throw std::runtime_error("function takes a parameter of an unknown type: " + m_type);
        }
    }

    RoutineParameter(RoutineParameter&& param) = default;
    std::string m_type;

    void print() override
    {
        cout << "VAR_PAR:" << m_name;
    }
};

class Routine : public Declaration
{
public:
    explicit Routine(std::string name) : Declaration(GrammarUnit::ROUTINE, std::move(name))
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        for (auto& param : m_params) {
            param->checkUndeclared(table);
        }

        if (!return_type.empty() && !table.contains(return_type)) {
            throw std::runtime_error("function returns an unknown type: " + return_type);
        }

        table.insert({m_name,  std::static_pointer_cast<Declaration>(shared_from_this())});

        m_body->checkUndeclared(table);
    }

    void print() override
    {
        cout << "Routine declaration, name: " << m_name << " -> " << (return_type.empty() ? "void" : return_type)
             << '\n';
        m_body->print();

        if (m_body->m_return.get())
        {
            cout << "Returns: ";
            m_body->m_return->print();
        }
        else
        {
            cout << "Returns nothing";
        }

        cout << "\n";
        cout << "End of routine declaration\n";
    }

    std::shared_ptr<Body> m_body;
    std::vector<std::shared_ptr<RoutineParameter>> m_params;
    std::string return_type;
};

class Type : public Declaration
{
public:
    ~Type() override = default;

    explicit Type(std::string typename_) : Declaration(GrammarUnit::TYPE, std::move(typename_))
    {
    }

    void print() override
    {
        cout << "TYPE:" << m_name << "\n";
    }
};

class Variable : public Declaration
{
public:
    explicit Variable(std::string name, std::shared_ptr<Type> type) : Declaration(GrammarUnit::VARIABLE, std::move(name)), m_type(std::move(type))
    {
    }

    std::shared_ptr<Expression> m_value;
    std::shared_ptr<Type> m_type;

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        if (!table.contains(m_type->m_name)) {
            throw std::runtime_error("unknown type of the variable or record field: " + m_type->m_name);
        }
    }

    void print() override
    {
        cout << "VAR:" << m_name << " ";
        if (m_value.get())
        {
            cout << "value: ";
            m_value->print();
        }
        cout << "\n";
    }
};

class RecordType : public Type
{
public:
    explicit RecordType(std::string name) : Type(std::move(name))
    {
    }

    std::vector<std::shared_ptr<Declaration>> m_fields;

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        for (auto& field : m_fields) {
            auto* var = dynamic_cast<Variable*>(field.get());
            var->m_type->checkUndeclared(table);
        }

        table.emplace(m_name, std::static_pointer_cast<Declaration>(shared_from_this()));
    }

    void print() override
    {
        cout << "TYPE_RECORD:" << m_name << ":\n";
        for (auto& field : m_fields)
        {
            cout << " ";
            field->print();
        }
        cout << "\n";
    }
};

/*
 *  Its primitive type and record types that have names, arrays dont...
*/
class PrimitiveType : public Type
{
    public:
    explicit PrimitiveType(std::string type) : Type(std::move(type)) {

    }
};

class ArrayType : public Type
{
public:
    explicit ArrayType(std::shared_ptr<Type> type, std::shared_ptr<Expression> size)
        : Type("array"), m_type(std::move(type)), m_size(size)
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        if (!table.contains(m_type->m_name)) {
            throw std::runtime_error("Unknown type: " + m_type->m_name);
        }
    }

    std::shared_ptr<Type> m_type;
    std::shared_ptr<Expression> m_size;

    void print() override
    {
        std::cout << " size: ";
        m_size->print();
        m_type->print();
    }
};

class TypeAliasing : public Type
{
public:
    explicit TypeAliasing(std::string from, std::string to) : Type(to), m_from(std::move(from)), m_to(std::move(to))
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        if (!table.contains(m_from)) {
            throw std::runtime_error("unknown type: " + m_from);
        }
        table.emplace(m_to, std::static_pointer_cast<Declaration>(shared_from_this()));
    }

    std::string m_from;
    std::string m_to;
};


class PrimitiveVariable : public Variable
{
public:
    void print() override
    {
        std::cout << "varname: " << m_name << " " << "type: " << m_type->m_name;
        if (m_assigned.get())
        {
            std::cout << " assigned ";
            m_assigned->print();
        }
        std::cout << "\n";
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        if (!table.contains(m_type->m_name)) {
            throw std::runtime_error("Unknown type: " + m_type->m_name);
        }
        table.emplace(m_name, table.at(m_type->m_name));
    }

    explicit PrimitiveVariable(std::string name, std::shared_ptr<Type> type) : Variable(std::move(name), std::move(type))
    {
    }

    PrimitiveVariable(std::string name, std::shared_ptr<Type> type, std::shared_ptr<Expression> expr)
        : Variable(std::move(name), std::move(type)), m_assigned(expr)
    {
    }

    std::shared_ptr<Expression> m_assigned;
};

class ArrayVariable : public Variable
{
public:
    explicit ArrayVariable(std::string name, std::shared_ptr<ArrayType> type)
        : Variable(std::move(name), type->m_type), m_type(type)
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        auto array_type = m_type->m_type;
        m_type->checkUndeclared(table);
        table.emplace(m_name, std::static_pointer_cast<Declaration>(shared_from_this()));
    }

    std::shared_ptr<ArrayType> m_type;

    void print() override
    {
        std::cout << "Array " << m_name << " ";
        m_type->print();
        std::cout << "\n";
    }
};

} // namespace parsing
