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
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    RoutineParameter(std::string name, std::string type)
        : Declaration(GrammarUnit::PARAMETER, std::move(name)), m_type(std::move(type))
    {
    }

    RoutineParameter(RoutineParameter&& param) = default;
    std::string m_type;
};

class Type : public Declaration
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

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    ~Type() override = default;

    explicit Type(std::string typename_) : Declaration(GrammarUnit::TYPE, std::move(typename_))
    {
    }

    virtual bool isRecord()
    {
        return false;
    }

    virtual bool isPrimitive()
    {
        if (m_name == "array")
        {
            return false;
        }
        return false;
    }

    virtual bool isArray()
    {
        if (m_name == "array")
        {
            return true;
        }
        return false;
    }

    virtual bool operator==(Type& other)
    {
        if (isArray() && other.isArray())
        {
            return false;
        }

        if (isRecord() && other.isRecord())
        {
            return m_name == other.m_name;
        }

        if (isPrimitive() && other.isPrimitive())
        {
            return true; // conversions, it's ok
        }

        return false;
    }

    virtual bool operator!=(Type& other)
    {
        return !(*this == other);
    }
};

class Variable : public Declaration
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

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit Variable(std::string name, std::shared_ptr<Type> type)
        : Declaration(GrammarUnit::VARIABLE, std::move(name)), m_type(std::move(type))
    {
    }

    std::shared_ptr<Expression> m_value;
    std::shared_ptr<Type> m_type;

    bool isVariableDecl() override
    {
        return true;
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override
    {
    }
};

class RecordType : public Type
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

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit RecordType(std::string name) : Type(std::move(name))
    {
    }

    std::vector<std::shared_ptr<Declaration>> m_fields;

    bool isRecord() override
    {
        return true;
    }
};

/*
 *  Its primitive type and record types that have names, arrays dont...
*/
class PrimitiveType : public Type
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

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit PrimitiveType(std::string type) : Type(std::move(type))
    {
    }

    bool isPrimitive() override
    {
        if (m_name == "array")
        {
            return false;
        }
        return true;
    }
};

class ArrayType : public Type
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

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit ArrayType(std::shared_ptr<Type> type, std::shared_ptr<Expression> size)
        : Type("array"), m_type(std::move(type)), m_size(size)
    {
    }

    std::shared_ptr<Type> m_type;
    std::shared_ptr<Expression> m_size;
    int m_generated_size;

    bool isArray() override
    {
        return true;
    }

    bool operator==(Type& other) override
    {
        try
        {
            auto& other_type = dynamic_cast<ArrayType&>(other);
            return *m_type == *other_type.m_type;
        }
        catch (const std::bad_cast& err)
        {
            std::cout << "attempt to assign non-array to an array: " + m_type->m_name + " " + " to " + other.m_name
                    + "\n";
            throw;
        }
    }
};

class TypeAliasing : public Type
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

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit TypeAliasing(std::shared_ptr<Type> from, std::string to) : Type(to), m_from(from), m_to(std::move(to))
    {
    }

    std::shared_ptr<Type> m_from;
    std::string m_to;
};

class PrimitiveVariable : public Variable
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

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override
    {
    }

    explicit PrimitiveVariable(std::string name, std::shared_ptr<Type> type)
        : Variable(std::move(name), std::move(type))
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
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit ArrayVariable(std::string name, std::shared_ptr<ArrayType> type)
        : Variable(std::move(name), type->m_type), m_type(type)
    {
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override
    {
    }

    std::shared_ptr<ArrayType> m_type;
};

} // namespace parsing
