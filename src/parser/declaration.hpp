#pragma once

#include "AST-node.hpp"
#include "grammar-units.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
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

    virtual bool isRecord() {
        return false;
    }

    virtual bool isPrimitive() {
        if (m_name == "array") {
            return false;
        }
        return false;
    }

    virtual bool isArray() {
        if (m_name == "array") {
            return true;
        }
        return false;
    }

    virtual bool operator==(Type& other) {
        if (isArray() && other.isArray()) {
            return *this == other;
        }

        if (isRecord() && other.isRecord()) {
            return m_name == other.m_name;
        }

        if (isPrimitive() && other.isPrimitive()) {
            return true;  // conversions, it's ok
        }

        return false;
    }

    virtual bool operator!=(Type& other) {
        return !(*this == other);
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
        table.emplace(m_name, table.at(m_type->m_name));

        auto assigned_type = m_value->deduceType(table);
        auto actual_type = std::static_pointer_cast<Type>(table.at(m_type->m_name));
        if (*assigned_type != *actual_type) {
            throw std::runtime_error("declared type and assigned values do not match: " + assigned_type->m_name + " " + m_type->m_name);
        }
    }

    bool isVariableDecl() override {
        return true;
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override {

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

    bool isRecord() override {
        return true;
    }

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

    bool isPrimitive() override {
        if (m_name == "array") {
            return false;
        }
        return true;
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

    bool isArray() override {
        return true;
    }

    bool operator==(Type& other) override {
        try {
            auto& other_type = dynamic_cast<ArrayType&>(other);
            return *m_type == *other_type.m_type;
        } 
        catch (const std::bad_cast& err) {
            std::cout << "attempt to assign non-array to an array: " + m_type->m_name + " " + " to " + other.m_name + "\n";
            throw;
        }
    }    

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
        table.emplace(m_to, std::static_pointer_cast<Declaration>(table.at(m_from)));
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
        if (m_assigned.get()) {
            m_assigned->checkUndeclared(table);
        }
        table.emplace(m_name, table.at(m_type->m_name));
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override {

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

    void removeUnused(std::unordered_map<std::string, int>& table) override {

    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        auto array_type = m_type->m_type;
        m_type->checkUndeclared(table);
        table.emplace(m_name, std::static_pointer_cast<ArrayType>(m_type));
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
