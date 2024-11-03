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
    void accept(IVisitor& visitor) override {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override {
        visitor.visit(*this);
    }

    explicit Integer(int value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::INTEGER;
    }

    int m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::INTEGER;
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        return std::static_pointer_cast<PrimitiveType>(table.at("integer"));
    }

};

class Boolean : public Primary
{
public:
    void accept(IVisitor& visitor) override {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override {
        visitor.visit(*this);
    }

    explicit Boolean(bool value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::BOOL;
    }

    bool m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::BOOL;
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        return std::static_pointer_cast<PrimitiveType>(table.at("boolean"));
    }

};

class Real : public Primary
{
public:
    void accept(IVisitor& visitor) override {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override {
        visitor.visit(*this);
    }

    explicit Real(double value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::REAL;
    }

    double m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::REAL;
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        return std::static_pointer_cast<PrimitiveType>(table.at("real"));
    }

};

struct Chained : public ASTNode
{
    Chained() : ASTNode(GrammarUnit::ACCESS_RECORD) {}
    virtual void check_has_field(std::shared_ptr<Declaration>& current_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) = 0;
    virtual std::shared_ptr<Type> deduceType(std::shared_ptr<Type> cur_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) = 0;
};

struct ArrayAccess : public Chained
{
    void accept(IVisitor& visitor) override {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override {
        visitor.visit(*this);
    }

    std::shared_ptr<Expression> access;

    void check_has_field(std::shared_ptr<Declaration>& current_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>&table) override {
        // skip, it is just an array access, the array 
        // identifier was before.
        // But we need to check that this is really an array...
        try {
            ArrayType& array = dynamic_cast<ArrayType&>(*current_type);
            if (array.m_type->m_name == "array") {
                current_type = array.m_type;
            } else {
                current_type = table.at(array.m_type->m_name);
            }
        } catch (const std::bad_cast& e) {
            std::cout << "Accessed type is not an array: " + current_type->m_name << '\n';
            throw;
        }
    }

    std::shared_ptr<Type> deduceType(std::shared_ptr<Type> cur_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>&) override {
        ArrayType& array = dynamic_cast<ArrayType&>(*cur_type);
        return array.m_type;
    }
};

struct RecordAccess : public Chained
{
    void accept(IVisitor& visitor) override {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override {
        visitor.visit(*this);
    }

    void check_has_field(std::shared_ptr<Declaration>& current_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        // a.b
        // so we are supposedly at 'a' now (in current type variable)
        // and b is the identifier
        try {
            RecordType& record = dynamic_cast<RecordType&>(*current_type);
            // check that record REALLY has that name
            for (auto& field : record.m_fields) {
                Variable& var = dynamic_cast<Variable&>(*field);
                if (var.m_name == identifier) {
                    current_type = table.at(var.m_type->m_name);
                    return;
                }
            }
            throw std::runtime_error("the field with name " + identifier +
                                            " is not found in record " + record.m_name);
        } catch (const std::bad_cast& e) {
            std::cout << "Accessed field is not a record: " + identifier;
            throw;
        }
    }

    std::shared_ptr<Type> deduceType(std::shared_ptr<Type> cur_type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        RecordType& record = static_cast<RecordType&>(*table.at(cur_type->m_name));
        // check that record REALLY has that name
        for (auto& field : record.m_fields) {
            Variable& var = dynamic_cast<Variable&>(*field);
            if (var.m_name == identifier) {
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
    void accept(IVisitor& visitor) override {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override {
        visitor.visit(*this);
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        std::shared_ptr<Type> cur_type = std::dynamic_pointer_cast<Type>(table.at(m_head_name));
        if (m_chain.empty()) {
            return cur_type;
        }
        for (auto& access : m_chain) {
            cur_type = access->deduceType(cur_type, table);
        }
        return std::dynamic_pointer_cast<Type>(table.at(cur_type->m_name));
    }

    void removeUnused(std::unordered_map<std::string, int>& outer_table) override {
        outer_table[m_head_name] += 1;
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        if (!table.contains(m_head_name)) {
            throw std::runtime_error("unknown identifier: " + m_head_name);
        }

        if (m_chain.empty()) {
            return;
        }

        auto type = table.at(m_head_name);
        // dealing with records.... or an array access... or both (
        for (auto& access : m_chain) {
            access->check_has_field(type, table);
        }
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
    void accept(IVisitor& visitor) override {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override {
        visitor.visit(*this);
    }

    explicit Math() : Expression()
    {
    }

    void checkUndeclared(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        m_left->checkUndeclared(table);
        m_right->checkUndeclared(table);
    }

    std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& table) override {
        auto left_type = m_left->deduceType(table);
        auto right_type = m_right->deduceType(table);

        if (*left_type != *right_type) {
            throw std::runtime_error("You are performing operation on types that do not match: " + left_type->m_name + " " + right_type->m_name);
        }
        return left_type;
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override {
        m_left->removeUnused(table);
        m_right->removeUnused(table);
    }

    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Expression> m_right;

};

class Plus : public Math
{
public:
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
    explicit Division() : Math()
    {
        this->m_grammar = GrammarUnit::DIVISION;
    }
};

class Logic : public Math
{
public:
    explicit Logic() : Math()
    {
    }
};

class And : public Logic
{
public:
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
    explicit Relation() : Math()
    {
    }
};

class Mod : public Relation
{
public:
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
