#pragma once

#include <memory>
#include "AST-node.hpp"
// #include "grammar-units.hpp"

namespace parsing {

class Primary : public Expression {

public:
    explicit Primary() : Expression() {}
    ~Primary() override = default;
};

class Integer : public Primary {

public:
    explicit Integer(int value) : Primary(), m_value(value) {}
    int m_value;
};

class Boolean : public Primary {

public:
    explicit Boolean(bool value) : Primary(), m_value(value) {}
    bool m_value;
};

class Real : public Primary {

public:
    explicit Real(double value) : Primary(), m_value(value) {}
    double m_value;
};

class Modifiable : public Primary {
public:

struct Chained { 
    virtual ~Chained() = default;
};
struct ArrayAccess : public Chained { ArrayAccess() = default; std::shared_ptr<Expression> access; };
struct RecordAccess : public Chained { RecordAccess() = default; std::string identifier; };

    explicit Modifiable(std::string head) : Primary(), m_head_name(head) {}
    std::vector<std::shared_ptr<Chained>> m_chain;
    std::string m_head_name;
};


class Math : public Expression {

public:
    explicit Math() : Expression() {}
    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Expression> m_right;
};

class Plus : public Math {

public:
    explicit Plus() : Math() {}
};

class Minus : public Math {

public:
    explicit Minus() : Math() {}
};

class Multiplication : public Math {

public:
    explicit Multiplication() : Math() {}
};

class Division : public Math {

public:
    explicit Division() : Math() {}
};

class Logic : public Math {

public:
    explicit Logic() : Math() {}
};

class And : public Logic {

public:
    explicit And() : Logic() {}
};

class Or : public Logic {

public:
    explicit Or() : Logic() {}
};

class Xor : public Logic {

public:
    explicit Xor() : Logic() {}
};

class Relation : public Math {

public:
    explicit Relation() : Math() {}
};

class Mod : public Relation {

public:
    explicit Mod() : Relation() {}
};

class Greater : public Relation {

public:
    explicit Greater() : Relation() {}
};

class Less : public Relation {

public:
    explicit Less() : Relation() {}
};

class GreaterEqual : public Relation {

public:
    explicit GreaterEqual() : Relation() {}
};

class LessEqual : public Relation {

public:
    explicit LessEqual() : Relation() {}
};

class Equal : public Relation {

public:
    explicit Equal() : Relation() {}
};

class NotEqual : public Relation {

public:
    explicit NotEqual() : Relation() {}
};

}  // namespace parsing
