#include "abstract_declaration.cpp"
#include "type.hpp"

template<typename T>
class Variable : public Declaration {
public:
    explicit Variable(string name, VarType type, Span* span): Declaration(name, Category::VARIABLE, span), type(type) {
        isAssigned = false;
    }
    VarType type;
    T value;
    bool isAssigned;

    void assign(T new_value) {
        this->value = new_value;
        this->isAssigned = true;
    }
};

class Function : public Declaration {
public:
    explicit Function(string name, Span* span, VarType input, VarType output):
        Declaration(name, Category::FUNCTION, span), input(input), output(output) {}
    VarType input, output;
};

template<typename T>
class Constant : public Declaration {
public:
    explicit Constant(VarType type, T value): Declaration(Category::CONSTANT, nullptr), type(type), value(value) {}
    VarType type;
    T value;
};

template<typename T>
class Array : public Declaration {
public:
    explicit Array(string name, VarType item_type, int length, Span* span):
        Declaration(name, Category::ARRAY, span), item_type(item_type), length(length) {
        isAssigned = false;
        values = new T[length];
    }

    VarType item_type;
    int length;
    bool isAssigned;
    T* values;
};

class Condition : public Declaration {
public:
    explicit Condition(Span* span): Declaration(Category::CONDITION, span) {
        statement = Tree<Declaration*>(nullptr);

        then_part = nullptr;
        else_part = nullptr;
    }
    Tree<Declaration*> statement;
    Declaration *then_part, *else_part;
};

class For : public Declaration {
public:
    explicit For(Span* span, string& iterator_name, int begin, int end):
            Declaration(Category::FOR, span), body(nullptr) {

        this->iterator = new Variable<int>(iterator_name, VarType::INT, span);
        this->iterator->assign(begin);

        this->begin = new Constant<int>(VarType::INT, begin);
        this->end = new Constant<int>(VarType::INT, end);
    }
    Variable<int> *iterator;
    Constant<int> *begin, *end;
    Declaration* body;
};

class While : public Declaration {
public:
    explicit While(Span* span, Condition *condition): Declaration(Category::WHILE, span),
            condition(condition) {

        body = nullptr;
    }
    Condition *condition;
    Declaration *body;
};

class Type : public Declaration {
public:
    // ...
};

class Expression : public Declaration {
public:
    // ... Tree?
};

class Brace : public Declaration {
public:
    explicit Brace(BracesType type, bool is_open, int code_line):
            Declaration(Category::BRACE, nullptr), is_open(is_open), type(type) {

        this->code_line = code_line;
    };
    bool is_open;
    BracesType type;
    int code_line;
};

class Puntuation : public Declaration {
public:
    explicit Puntuation(PunctuationType type):
        Declaration(Category::PUNCTUATION, nullptr), type(type) {};
    PunctuationType type;
};

class Assignment : public Declaration {
public:
    explicit Assignment(Declaration* left_part):
        Declaration(Category::ASSIGNMENT, nullptr), left_part(left_part), right_part(nullptr) {};

    explicit Assignment(Declaration* left_part, Declaration* right_part):
            Declaration(Category::ASSIGNMENT, nullptr), left_part(left_part), right_part(right_part) {};

    Declaration* left_part;
    Declaration* right_part;
};

class Record : public Declaration {
public:
    explicit Record(string& name, Span* span): Declaration(name, Category::RECORD, span),
        fields(nullptr), methods(nullptr) {};

    Declaration* fields;
    Function* methods;
};
