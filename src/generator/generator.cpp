#include "generator.hpp"

#include "parser/visitor/abstract-visitor.hpp"
#include "parser/statement.hpp"

namespace generator {

    llvm::Value* Generator::generateExpression(std::shared_ptr<parsing::Expression> expression) {
        // TODO: generate expression code for an expression node

        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 1);
    }

    // TODO: translate AST nodes to LLVM nodes

    void Generator::visit(parsing::If& node) {
        // Go the parent function (last point) inside CFG
        llvm::Function* parent_func = builder.GetInsertBlock()->getParent();

        llvm::Value* cond = generateExpression(node.m_condition);

        llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", parent_func);
        llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else");
        llvm::BasicBlock* contBB = llvm::BasicBlock::Create(context, "ifcont");

        // Creating IF branch
        builder.CreateCondBr(cond, thenBB, elseBB);

        // THEN part
        builder.SetInsertPoint(thenBB);
        node.m_then->accept(*this);
        builder.CreateBr(contBB);

        // ELSE part
        parent_func->getBasicBlockList().push_back(elseBB);
        builder.SetInsertPoint(elseBB);
        if (node.m_else) {
            node.m_else->accept(*this);
        }
        builder.CreateBr(contBB);

        parent_func->getBasicBlockList().push_back(contBB);
        builder.SetInsertPoint(contBB);
    }

    void Generator::visit(parsing::ASTNode& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Declaration& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Type& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::RecordType& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::ArrayType& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Variable& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::ArrayVariable& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::PrimitiveVariable& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Body& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Routine& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::RoutineCall& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::RoutineCallResult& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::RoutineParameter& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Statement& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Expression& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Modifiable& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::ReturnStatement& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Range& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::For& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::While& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Assignment& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Program& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::True& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::False& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Math& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Real& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Boolean& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::Integer& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::ArrayAccess& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::RecordAccess& node) {
        // Target llvm:: ...
    }

    void Generator::visit(parsing::TypeAliasing& node) {
        // Target llvm:: ...
    }

} // namespace generator
