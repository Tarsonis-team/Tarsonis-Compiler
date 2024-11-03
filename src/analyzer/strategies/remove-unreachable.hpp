#pragma once

#include "parser/AST-node.hpp"
#include "parser/visitor/abstract-visitor.hpp"
#include "parser/declaration.hpp"
#include "parser/statement.hpp"
#include "parser/expression.hpp"
#include "parser/return.hpp"
#include "parser/body.hpp"
#include "parser/routine.hpp"
#include <memory>


struct RemoveUnreachableCode : public parsing::IVisitor
{

explicit RemoveUnreachableCode(std::shared_ptr<parsing::Program> program) : m_ast(program) {}

void visit(parsing::ASTNode& node) override {
    // this enables double-dispatching in our code.
    node.accept(*this);
}

void visit(parsing::Program& node) override {
    for (auto& entity : node.m_declarations) {
        entity->accept(*this);
    }
}

void visit(parsing::Declaration& node) override {

}

void visit(parsing::Type& node) override {
}

void visit(parsing::TypeAliasing& node) override {
}

void visit(parsing::ArrayType& node) override {

}

void visit(parsing::ArrayVariable& node) override {

}

void visit(parsing::PrimitiveVariable& node) override {
}

void visit(parsing::Body& node) override {
    size_t idx_of_return = node.m_items.size();
    for (size_t idx = 0; idx < node.m_items.size(); ++idx) {
        if (node.m_items[idx]->m_grammar == GrammarUnit::RETURN) {
            idx_of_return = idx + 1;
            break;
        }
        node.m_items[idx]->accept(*this);
    }

    node.m_items.erase(node.m_items.begin() + idx_of_return, node.m_items.end());
}

void visit(parsing::Routine& node) override {
    node.m_body->accept(*this);
}

void visit(parsing::RoutineCall& node) override {

}

void visit(parsing::RoutineCallResult& node) override {

}

void visit(parsing::RoutineParameter& node) override {
}

void visit(parsing::Statement& node) override {

}

void visit(parsing::Expression& node) override {

}

void visit(parsing::True&) override {
}

void visit(parsing::False&) override {
}

void visit(parsing::Math& node) override {
}

void visit(parsing::Real& node) override {
}

void visit(parsing::Boolean& node) override {
}

void visit(parsing::Integer& node) override {
}

void visit(parsing::Modifiable& node) override {

}

void visit(parsing::ArrayAccess& node) override {

}

void visit(parsing::RecordAccess& node) override {
}

void visit(parsing::ReturnStatement& node) override {
}

void visit(parsing::If& node) override {
    node.m_then->accept(*this);
    if (node.m_else.get()) {
        node.m_else->accept(*this);
    }
}

void visit(parsing::Range& node) override {

}

void visit(parsing::For& node) override {
    node.m_body->accept(*this);
}

void visit(parsing::While& node) override {
    node.m_body->accept(*this);
}

void visit(parsing::Assignment& node) override {

}

void visit(parsing::RecordType& node) override {

}

void apply()
{
    m_ast->accept(*this);
}

    std::shared_ptr<parsing::Program> m_ast;
};
