#pragma once

#include "AST-node.hpp"
#include "statement.hpp"
#include <memory>

namespace parsing
{

class Print : public RoutineCall
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit Print() : RoutineCall("print") {}
};

} // namespace parsing