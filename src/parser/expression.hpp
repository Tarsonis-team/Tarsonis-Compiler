#pragma once

#include "AST-node.hpp"
#include "declaration.hpp"
#include "grammar-units.hpp"

#include <memory>
#include <stdexcept>

namespace parsing
{
struct IVisitor;

class Primary : public Expression
{
public:
    explicit Primary() : Expression()
    {
    }

    ~Primary() override = default;
};

class Integer : public Primary
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

    explicit Integer(int value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::INTEGER;
    }

    bool isConst() override {
        return true;
    }

    int m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::INTEGER;
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>&,
    std::unordered_map<std::string, std::shared_ptr<Declaration>>& type_table) override
    {
        return std::static_pointer_cast<PrimitiveType>(type_table.at("integer"));
    }
};

class Boolean : public Primary
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

    explicit Boolean(bool value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::BOOL;
    }

    bool isConst() override {
        return true;
    }

    bool m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::BOOL;
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>&,
    std::unordered_map<std::string, std::shared_ptr<Declaration>>& type_table) override
    {
        return std::static_pointer_cast<PrimitiveType>(type_table.at("boolean"));
    }
};

class Real : public Primary
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

    explicit Real(double value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::REAL;
    }

    bool isConst() override {
        return true;
    }

    double m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::REAL;
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>&,
    std::unordered_map<std::string, std::shared_ptr<Declaration>>& type_table) override
    {
        return std::static_pointer_cast<PrimitiveType>(type_table.at("real"));
    }
};

struct Chained : public ASTNode
{
    Chained() : ASTNode(GrammarUnit::ACCESS_RECORD)
    {
    }

    virtual void check_has_field(
        std::shared_ptr<Declaration>& current_type,
        std::unordered_map<std::string, std::shared_ptr<Declaration>>& table)
        = 0;
    virtual std::shared_ptr<Type>
    deduceType(std::shared_ptr<Type> cur_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table)
        = 0;
};

struct ArrayAccess : public Chained
{
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    std::shared_ptr<Expression> access;

    void check_has_field(
        std::shared_ptr<Declaration>& current_type,
        std::unordered_map<std::string, std::shared_ptr<Declaration>>&) override
    {
        // skip, it is just an array access, the array
        // identifier was before.
        // But we need to check that this is really an array...
        try
        {
            ArrayType& array = dynamic_cast<ArrayType&>(*current_type);
            current_type = array.m_type;
        }
        catch (const std::bad_cast& e)
        {
            std::cout << "Accessed type is not an array: " + current_type->m_name << '\n';
            throw;
        }
    }

    std::shared_ptr<Type>
    deduceType(std::shared_ptr<Type> cur_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>&) override
    {
        ArrayType& array = dynamic_cast<ArrayType&>(*cur_type);
        return array.m_type;
    }
};

struct RecordAccess : public Chained
{
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void check_has_field(
        std::shared_ptr<Declaration>& current_type,
        std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override
    {
        // a.b
        // so we are supposedly at 'a' now (in current type variable)
        // and b is the identifier
        try
        {
            RecordType& record = dynamic_cast<RecordType&>(*table.at(current_type->m_name));
            // check that record REALLY has that name
            for (auto& field : record.m_fields)
            {
                Variable& var = dynamic_cast<Variable&>(*field);
                if (var.m_name == identifier)
                {
                    current_type = table.at(var.m_type->m_name);
                    return;
                }
            }
            throw std::runtime_error("the field with name " + identifier + " is not found in record " + record.m_name);
        }
        catch (const std::bad_cast& e)
        {
            std::cout << "Accessed field is not a record: " + identifier;
            throw;
        }
    }

    std::shared_ptr<Type> deduceType(
        std::shared_ptr<Type> cur_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override
    {
        RecordType& record = static_cast<RecordType&>(*table.at(cur_type->m_name));
        // check that record REALLY has that name
        for (auto& field : record.m_fields)
        {
            Variable& var = dynamic_cast<Variable&>(*field);
            if (var.m_name == identifier)
            {
                return std::dynamic_pointer_cast<Type>(table.at(var.m_type->m_name));
            }
        }
        throw std::runtime_error("field not found !");
    }

    std::string identifier;
};

class Modifiable : public Primary
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

    bool isConst() override {
        return false;
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& var_table,
    std::unordered_map<std::string, std::shared_ptr<Declaration>>& type_table) override
    {
        std::shared_ptr<Type> cur_type = std::dynamic_pointer_cast<Type>(var_table.at(m_head_name));
        if (m_chain.empty())
        {
            return cur_type;
        }
        for (auto& access : m_chain)
        {
            cur_type = access->deduceType(cur_type, type_table);
        }
        return std::dynamic_pointer_cast<Type>(type_table.at(cur_type->m_name));
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override
    {
        outer_table[m_head_name] += 1;
    }

    explicit Modifiable(std::string head) : Primary(), m_head_name(head)
    {
        this->m_grammar = GrammarUnit::IDENTIFIER;
    }

    explicit Modifiable() : Primary()
    {
        this->m_grammar = GrammarUnit::IDENTIFIER;
    }

    std::vector<std::shared_ptr<Chained>> m_chain;
    std::string m_head_name;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::IDENTIFIER;
    }
};

class Math : public Expression
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

    bool isConst() override {
        return m_left->isConst() && m_right->isConst();
    }

    explicit Math() : Expression()
    {
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& var_table,
    std::unordered_map<std::string, std::shared_ptr<Declaration>>& type_table) override
    {
        auto left_type = m_left->deduceType(var_table, type_table);
        auto right_type = m_right->deduceType(var_table, type_table);

        if (*left_type != *right_type)
        {
            throw std::runtime_error(
                "You are performing operation on types that do not match: " + left_type->m_name + " "
                + right_type->m_name);
        }
        return left_type;
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override
    {
        m_left->removeUnused(table);
        m_right->removeUnused(table);
    }

    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Expression> m_right;
};

class Plus : public Math
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

    explicit Plus() : Math()
    {
        this->m_grammar = GrammarUnit::PLUS;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::PLUS;
    }
};

class Minus : public Math
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
    
    explicit Minus() : Math()
    {
        this->m_grammar = GrammarUnit::MINUS;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::MINUS;
    }
};

class Multiplication : public Math
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
    
    explicit Multiplication() : Math()
    {
        this->m_grammar = GrammarUnit::MULTIPLICATE;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::MULTIPLICATE;
    }
};

class Division : public Math
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
    
    explicit Division() : Math()
    {
        this->m_grammar = GrammarUnit::DIVISION;
    }
};

class Logic : public Math
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
    
    explicit Logic() : Math()
    {
    }
};

class And : public Logic
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
    
    explicit And() : Logic()
    {
        this->m_grammar = GrammarUnit::AND;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::AND;
    }
};

class Or : public Logic
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
    
    explicit Or() : Logic()
    {
        this->m_grammar = GrammarUnit::OR;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::OR;
    }
};

class Xor : public Logic
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
    
    explicit Xor() : Logic()
    {
        this->m_grammar = GrammarUnit::XOR;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::XOR;
    }
};

class True : public Boolean
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
    
    explicit True() : Boolean(true)
    {
        this->m_grammar = GrammarUnit::TRUE;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::TRUE;
    }
};

class False : public Boolean
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
    
    explicit False() : Boolean(false)
    {
        this->m_grammar = GrammarUnit::FALSE;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::FALSE;
    }
};

class Relation : public Math
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
    
    explicit Relation() : Math()
    {
    }
};

class Mod : public Relation
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
    
    explicit Mod() : Relation()
    {
        this->m_grammar = GrammarUnit::MOD;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::MOD;
    }
};

class Greater : public Relation
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
    
    explicit Greater() : Relation()
    {
        this->m_grammar = GrammarUnit::GREATER;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::GREATER;
    }
};

class Less : public Relation
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
    
    explicit Less() : Relation()
    {
        this->m_grammar = GrammarUnit::LESS;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::LESS;
    }
};

class GreaterEqual : public Relation
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
    
    explicit GreaterEqual() : Relation()
    {
        this->m_grammar = GrammarUnit::GREATER_EQUAL;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::GREATER_EQUAL;
    }
};

class LessEqual : public Relation
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
    
    explicit LessEqual() : Relation()
    {
        this->m_grammar = GrammarUnit::LESS_EQUAL;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::LESS_EQUAL;
    }
};

class Equal : public Relation
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
    
    explicit Equal() : Relation()
    {
        this->m_grammar = GrammarUnit::EQUAL;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::EQUAL;
    }
};

class NotEqual : public Relation
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
    
    explicit NotEqual() : Relation()
    {
        this->m_grammar = GrammarUnit::NOT_EQUAL;
    }

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::NOT_EQUAL;
    }
};

} // namespace parsing
