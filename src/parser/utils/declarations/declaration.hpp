#include "AST-node.hpp"
#include "type.hpp"

#include <vector>
#include <string>
#include <memory>

using namespace parsing;

class Variable : public Declaration {

public:
    explicit Variable(std::string& name) : Declaration(GrammarUnit::VARIABLE, name) {}
    std::weak_ptr<Expression> m_value; // ?
    VarType m_type;
};

class RoutineParameter : public Declaration {

public:
    explicit RoutineParameter(std::string& name) : Declaration(GrammarUnit::PARAMETER, name) {}
    std::weak_ptr<Expression> m_value; // ?
    VarType m_type;
};

class Routine : public Declaration {

public:
    explicit Routine(std::string& name) : Declaration(GrammarUnit::ROUTINE, name) {}
    std::unique_ptr<Body> m_body;
    std::vector<std::unique_ptr<RoutineParameter>> m_params;
    VarType m_return_type;
};


class Record : public Declaration {
public:
    explicit Record(std::string& name) : Declaration(GrammarUnit::RECORD, name) {}
    std::vector<std::unique_ptr<Declaration>> m_fields;
};

class Type : public Declaration {
public:
    explicit Type(std::string& name) : Declaration(GrammarUnit::TYPE, name) {}
    std::unique_ptr<Declaration> m_original;
};

class Array : public Declaration {
public:
    explicit Array(std::string& name, int size) : Declaration(GrammarUnit::ARRAY, name), m_size(size) {}
    std::vector<std::unique_ptr<Declaration>> m_items;
    int m_size;
    VarType m_items_type;
};



