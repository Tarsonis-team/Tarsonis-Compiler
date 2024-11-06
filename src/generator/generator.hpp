#include "parser/visitor/abstract-visitor.hpp"

namespace generator {
    struct Generator : parsing::IVisitor {
        // TODO: map<string, llvm::Type> m_type_table
        // TODO: map<string, llvm::Value> m_var_table

        // TODO: vector<llvm::Block> m_cfg


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
    };
} // namespace generator
