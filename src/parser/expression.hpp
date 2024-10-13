#pragma once

#include <memory>
#include "AST-node.hpp"
#include "declaration.hpp"

namespace parsing {

class Primary : public Expression {

public:
    explicit Primary(GrammarUnit gr) : Expression(gr) {}
};

class Integer : public Primary {

public:
    explicit Integer(int value) : Primary(GrammarUnit::INTEGER), m_value(value) {}
    int m_value;
};

class Boolean : public Primary {

public:
    explicit Boolean(bool value) : Primary(GrammarUnit::BOOL), m_value(value) {}
    bool m_value;
};

class Real : public Primary {

public:
    explicit Real(double value) : Primary(GrammarUnit::REAL), m_value(value) {}
    double m_value;
};

class Modifiable : public Primary {
public:

struct Chained { 
    virtual ~Chained() = default;
};
struct ArrayAccess : public Chained { ArrayAccess() = default; std::shared_ptr<Expression> access; };
struct RecordAccess : public Chained { RecordAccess() = default; std::string identifier; };

    explicit Modifiable(std::string head) : Primary(GrammarUnit::VARIABLE), m_head_name(head) {}
    std::vector<std::shared_ptr<Chained>> m_chain;
    std::string m_head_name;
};


class Math : public Expression {

public:
    explicit Math(GrammarUnit gr) : Expression(gr) {}
    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Expression> m_right;
};

class Plus : public Math {

public:
    explicit Plus() : Math(GrammarUnit::PLUS) {}
};

class Minus : public Math {

public:
    explicit Minus() : Math(GrammarUnit::MINUS) {}
};

class Multiplication : public Math {

public:
    explicit Multiplication() : Math(GrammarUnit::MULTIPLICATE) {}
};

class Division : public Math {

public:
    explicit Division() : Math(GrammarUnit::DIVISION) {}
};

class Logic : public Math {

public:
    explicit Logic(GrammarUnit gr) : Math(gr) {}
};

class And : public Logic {

public:
    explicit And() : Logic(GrammarUnit::AND) {}
};

class Or : public Logic {

public:
    explicit Or() : Logic(GrammarUnit::OR) {}
};

class Xor : public Logic {

public:
    explicit Xor() : Logic(GrammarUnit::XOR) {}
};

class Relation : public Math {

public:
    explicit Relation(GrammarUnit gr) : Math(gr) {}
};

class Mod : public Relation {

public:
    explicit Mod() : Relation(GrammarUnit::MOD) {}
};

class Greater : public Relation {

public:
    explicit Greater() : Relation(GrammarUnit::GREATER) {}
};

class Less : public Relation {

public:
    explicit Less() : Relation(GrammarUnit::LESS) {}
};

class GreaterEqual : public Relation {

public:
    explicit GreaterEqual() : Relation(GrammarUnit::GREATER_EQUAL) {}
};

class LessEqual : public Relation {

public:
    explicit LessEqual() : Relation(GrammarUnit::LESS_EQUAL) {}
};

class Equal : public Relation {

public:
    explicit Equal() : Relation(GrammarUnit::EQUAL) {}
};

class NotEqual : public Relation {

public:
    explicit NotEqual() : Relation(GrammarUnit::NOT_EQUAL) {}
};

}  // namespace parsing
