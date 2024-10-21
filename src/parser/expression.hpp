#pragma once

#include "AST-node.hpp"
#include "grammar-units.hpp"
#include <memory>

namespace parsing
{

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
    explicit Integer(int value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::INTEGER;
    }

    int m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::INTEGER;
    }

    void print() override
    {
        std::cout << "INTEGER:" << this->m_value;
    }
};

class Boolean : public Primary
{
public:
    explicit Boolean(bool value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::BOOL;
    }

    bool m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::BOOL;
    }

    void print() override
    {
        std::cout << "BOOL:" << this->m_value;
    }
};

class Real : public Primary
{
public:
    explicit Real(double value) : Primary(), m_value(value)
    {
        this->m_grammar = GrammarUnit::REAL;
    }

    double m_value;

    GrammarUnit get_grammar() const override
    {
        return GrammarUnit::REAL;
    }

    void print() override
    {
        std::cout << "REAL:" << this->m_value;
    }
};

class Modifiable : public Primary
{
public:
    struct Chained
    {
        virtual ~Chained() = default;
        virtual void print() = 0;
    };

    struct ArrayAccess : public Chained
    {
        ArrayAccess() = default;
        std::shared_ptr<Expression> access;

        void print() override
        {
            std::cout << "[ ";
            access->print();
            std::cout << " ]";
        }
    };

    struct RecordAccess : public Chained
    {
        void print() override
        {
            std::cout << "." + identifier;
        }

        RecordAccess() = default;
        std::string identifier;
    };

    void print() override
    {
        std::cout << m_head_name;
        for (auto& chain : m_chain)
        {
            chain->print();
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
    explicit Math() : Expression()
    {
    }

    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Expression> m_right;

    void print() override
    {
        std::cout << this->gr_to_str() << " with params: {" << std::to_string(static_cast<int>(this->m_grammar)) << "::\n";

        std::cout << " ";
        this->m_left->print();

        std::cout << ", ";
        this->m_right->print();

        std::cout << " ::" << std::to_string(static_cast<int>(this->m_grammar)) << "}";
    }
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

    void print() override
    {
        cout << "TRUE ";
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

    void print() override
    {
        cout << "FALSE ";
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
