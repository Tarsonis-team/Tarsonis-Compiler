#include "parser/visitor/abstract-visitor.hpp"
#include "parser/expression.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

#include <memory>
#include <unordered_map>

namespace generator {
struct Generator : public parsing::IVisitor {
    llvm::LLVMContext context{};
    std::shared_ptr<llvm::Module> module;

    // Something like Control Flow Graph
    llvm::IRBuilder<> builder;

    std::unordered_map<std::string, llvm::Type*> m_type_table;
    std::unordered_map<std::string, llvm::Value*> m_var_table;

    llvm::Value* current_expression;
    llvm::Function* current_function;

    explicit Generator() : module(std::make_shared<llvm::Module>("I_module", context)), builder(context) {}

    llvm::Type* typenameToType(const std::string& name);
    void gen_expr_fork(parsing::Math& node, llvm::Value*& left, llvm::Value*& right);

    void visit(parsing::ASTNode& node) override;
    void visit(parsing::Declaration& node) override;
    void visit(parsing::Type& node) override;
    void visit(parsing::RecordType& node) override;
    void visit(parsing::ArrayType& node) override;
    void visit(parsing::Variable& node) override;
    void visit(parsing::ArrayVariable& node) override;
    void visit(parsing::PrimitiveVariable& node) override;
    void visit(parsing::Body& node) override;
    void visit(parsing::Routine& node) override;
    void visit(parsing::RoutineCall& node) override;
    void visit(parsing::RoutineCallResult& node) override;
    void visit(parsing::RoutineParameter& node) override;
    void visit(parsing::Statement& node) override;
    void visit(parsing::Expression& node) override;
    void visit(parsing::Modifiable& node) override;
    void visit(parsing::ReturnStatement& node) override;
    void visit(parsing::Range& node) override;
    void visit(parsing::For& node) override;
    void visit(parsing::While& node) override;
    void visit(parsing::If& node) override;
    void visit(parsing::Assignment& node) override;
    void visit(parsing::Program& node) override;
    void visit(parsing::True& node) override;
    void visit(parsing::False& node) override;
    void visit(parsing::Math& node) override;
    void visit(parsing::Real& node) override;
    void visit(parsing::Boolean& node) override;
    void visit(parsing::Integer& node) override;
    void visit(parsing::ArrayAccess& node) override;
    void visit(parsing::RecordAccess& node) override;
    void visit(parsing::TypeAliasing& node) override;

    void visit(parsing::Plus& node);
    void visit(parsing::Minus& node);
    void visit(parsing::Multiplication& node);
    void visit(parsing::Division& node);
    void visit(parsing::And& node);
    void visit(parsing::Or& node);
    void visit(parsing::Xor& node);
    void visit(parsing::Mod& node);
    void visit(parsing::Greater& node);
    void visit(parsing::Less& node);
    void visit(parsing::GreaterEqual& node);
    void visit(parsing::LessEqual& node);
    void visit(parsing::Equal& node);
    void visit(parsing::NotEqual& node);
};
} // namespace generator
