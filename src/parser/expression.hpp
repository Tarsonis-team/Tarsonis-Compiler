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
    explicit Modifiable(GrammarUnit gr) : Primary(gr) {}
};

class AccessArray : public Modifiable {

public:
    explicit AccessArray(Array& array) : Modifiable(GrammarUnit::ACCESS_ARRAY), m_array(&array) {}
    std::unique_ptr<Array> m_array;
    std::unique_ptr<Expression> m_accessor;
};

class AccessRecord : public Modifiable {

public:
    explicit AccessRecord(Record& record) : Modifiable(GrammarUnit::ACCESS_RECORD), m_record(&record) {}
    std::unique_ptr<Record> m_record;
    std::unique_ptr<Identifier> m_field;
};

class Math : public Expression {

public:
    explicit Math(GrammarUnit gr) : Expression(gr) {}
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
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

class Relation : public Math {

public:
    explicit Relation(GrammarUnit gr) : Math(gr) {}
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
