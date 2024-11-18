#pragma once

#include "abstract-visitor.hpp"

#include "parser/AST-node.hpp"
#include "parser/declaration.hpp"
#include "parser/statement.hpp"
#include "parser/expression.hpp"
#include "parser/return.hpp"
#include "parser/body.hpp"
#include "parser/routine.hpp"
#include "parser/std-function.hpp"

namespace parsing {

using std::cout;

struct nest {
    int nest = 0;
    struct nest operator++() {
        nest++;
        return *this;
    }
    struct nest operator--() {
        nest--;
        return *this;
    }
};

std::ostream& operator<<(std::ostream& os, const nest& n) {
    for (int i = 0; i < n.nest * 4; ++i) {
        os << " ";
    }
    return os;
}

struct Printer : public IVisitor {

/*
 * Consider bringing this to CRTP.
*/
void visit(ASTNode& node) override {
    cout << "Some AST-Node, the default print function\n";
    // this enables double-dispatching in our code.
    node.accept(*this);
}

void visit(Program& node) override {
    cout << "Beginning of the Program:\n";
    for (auto& declaration : node.m_declarations)
    {
        declaration->accept(*this);
    }
    cout << "End of the program\n";
}

void visit(Declaration& node) override {

}

void visit(Type& node) override {
    cout << "type: " << node.m_name;
}

void visit(TypeAliasing& node) override {
    cout << m_nest << "type: " << node.m_from->m_name << " as " << node.m_to << '\n';
}

void visit(ArrayType& node) override {
    node.m_type->accept(*this);
    cout << "[";
    node.m_size->accept(*this); 
    cout << "]";
}

void visit(Variable& node) override {
    
}

void visit(ArrayVariable& node) override {
    std::cout << m_nest << node.m_name << " ";
    node.m_type->accept(*this);
    std::cout << "\n";
}

void visit(PrimitiveVariable& node) override {
    cout << m_nest << "var: " << node.m_name << " " << "type: " << node.m_type->m_name;
    if (node.m_assigned.get())
    {
        std::cout << " = ";
        node.m_assigned->accept(*this);
    }
    cout << "\n";
}

void visit(Body& node) override {
    std::cout << m_nest << "{\n";
    ++m_nest;
    for (auto& item : node.m_items)
    {
        item->accept(*this);
    }
    --m_nest;
    std::cout << m_nest << "}\n";
}

void visit(Routine& node) override {
    cout << "function declaration, name: " << node.m_name << " -> " << (node.return_type.empty() ? "void" : node.return_type)
            << '\n';
    node.m_body->accept(*this);

    cout << "\n";
}

void visit(RoutineCall& node) override {
    cout << m_nest << node.m_routine_name << " ( ";

    for (auto& par : node.m_parameters)
    {
        par->accept(*this);
        cout << ", ";
    }
    cout << ") \n";
}

void visit(RoutineCallResult& node) override {
    cout << node.m_routine_call->m_routine_name << " ( ";

    for (auto& par : node.m_routine_call->m_parameters)
    {
        par->accept(*this);
        cout << ", ";
    }
    cout << ") ";
}

void visit(RoutineParameter& node) override {
    cout << node.m_name << " " << node.m_type;
}

void visit(Statement& node) override {

}

void visit(Expression& node) override {

}

void visit(True&) override {
    cout << "True";
}

void visit(False&) override {
    cout << "False";
}

void visit(Math& node) override {
    std::cout << node.gr_to_str() << " with params: {" << std::to_string(static_cast<int>(node.m_grammar))
                << "::";

    std::cout << " ";
    node.m_left->accept(*this);

    std::cout << ", ";
    node.m_right->accept(*this);

    std::cout << " ::" << std::to_string(static_cast<int>(node.m_grammar)) << "}";
}

void visit(Real& node) override {
    cout << "real: " << node.m_value;
}

void visit(Boolean& node) override {
    std::cout << "bool: " << node.m_value;
}

void visit(Integer& node) override {
    std::cout << "int: " << node.m_value;
}

void visit(Modifiable& node) override {
    std::cout << node.m_head_name;
    for (auto& chain : node.m_chain)
    {
        chain->accept(*this);
    }
}

void visit(ArrayAccess& node) override {
    std::cout << "[ ";
    node.access->accept(*this);
    std::cout << " ]";
}

void visit(RecordAccess& node) override {
    cout << "." + node.identifier;
}

void visit(ReturnStatement& node) override {
    std::cout << m_nest << "RETURN "; 
    node.m_expr->accept(*this); 
    std::cout << "\n";
}

void visit(If& node) override {
    std::cout << m_nest << "if ";
    node.m_condition->accept(*this);
    std::cout << "\n";

    node.m_then->accept(*this);

    if (node.m_else.get())
    {
        std::cout << m_nest << "else \n";
        node.m_else->accept(*this);
    }
}

void visit(Range& node) override {
    std::cout << "in range";
    if (node.m_reverse)
    {
        std::cout << " (reverse)";
    }
    std::cout << ": ";

    node.m_begin->accept(*this);
    std::cout << " .. ";
    node.m_end->accept(*this);
}

void visit(For& node) override {
    cout << m_nest << "for ";
    cout << node.m_identifier->m_name;
    cout << " in ";
    node.m_range->accept(*this);
    cout << "\n";
    node.m_body->accept(*this);
}

void visit(While& node) override {
    std::cout << m_nest << "while ";
    node.m_condition->accept(*this);
    node.m_body->accept(*this);
}

void visit(Assignment& node) override {
    std::cout << m_nest;
    node.m_modifiable->accept(*this);
    std::cout << " = ";
    node.m_expression->accept(*this);
    std::cout << "\n";
}

void visit(RecordType& node) override {
    cout << "RECORD " << node.m_name << " { \n";
    for (auto& field : node.m_fields)
    {
        cout << "  ";
        field->accept(*this);
    }
    cout << "} \n";
}

void visit(StdFunction& node) override {
    cout << m_nest << "std::" << node.m_routine_name << " ( ";

    for (auto& par : node.m_parameters)
    {
        par->accept(*this);
        cout << ", ";
    }
    cout << ") \n";
}

    nest m_nest;
};

}  // namespace parsing
